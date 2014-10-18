#include "v2n-client.h"
#include <sstream>
#include <time.h>
#include <stdexcept>
#include <QDebug>

#define REQUEST_TIMEOUT     25      // msecs
#define REQUEST_RETRIES      3      // before we abandon
#define MAX_SEQNUM         255      // max sequence number

V2NClient::V2NClient() {

   last_perr = NONE;
   last_verr = cvSuccess;

   zmq_ctx = new zmq::context_t(1);
   is_online = false;
}

void V2NClient::setup(QString _hostname, uint16_t _port) {

   hostname = _hostname;
   port = _port;
}

QString V2NClient::gethostname(void) {

   return(hostname);
}

uint16_t V2NClient::getport(void) {

   return(port);
}

bool V2NClient::connect(void) {

   sockptr.reset(new zmq::socket_t(*zmq_ctx, ZMQ_REQ));

   zmq_client = sockptr.get();

   try {

      zmq_client->connect(QString("tcp://" + hostname + ":" + QString::number(port)).toStdString().c_str());

   } catch(const zmq::error_t& zerr) {

      last_zmqerr = zerr;
   }

   // configure socket to not wait at close time
   int linger = 0;
   zmq_client->setsockopt (ZMQ_LINGER, &linger, sizeof(linger));

   return(zmq_client->connected());
}

bool V2NClient::online(void) {

   return(is_online);
}

bool V2NClient::disconnect(void) {

   if(zmq_client->connected())
      zmq_client->disconnect(QString("tcp://" + hostname + ":" + QString::number(port)).toStdString().c_str());

   return(true);
}

void V2NClient::close(void) {

   sockptr.reset();
   zmq_client = NULL;
   zmq_ctx->close();
}

response_t V2NClient::access(request_t req) {

   response_t res;

   srand(time(NULL));
   req.seqnum = within(MAX_SEQNUM);

   req.am = cvA32_U_DATA;  // to fix
   req.dw = cvD16;         // to fix
   req.perr = NONE;

   std::string string_req = serialize_req(req);

   if(zmq_client->connected()) {

      int retries_left = REQUEST_RETRIES;
      while(retries_left) {

         zmq::message_t msg_req(string_req.size());
         memcpy(msg_req.data(), string_req.data(), string_req.size());
         zmq_client->send(msg_req, ZMQ_DONTWAIT);

         bool expect_reply = true;
         while (expect_reply) {

            // poll socket for a reply, with timeout
            zmq::pollitem_t items[] = { { *zmq_client, 0, ZMQ_POLLIN, 0 } };
            zmq::poll(&items[0], 1, REQUEST_TIMEOUT * 10);

            // if we got a reply, process it
            if (items[0].revents & ZMQ_POLLIN) {

               is_online = true;

               zmq::message_t msg_res;
               zmq_client->recv(&msg_res);

               std::string string_res = std::string(static_cast<char*>(msg_res.data()), msg_res.size());
               res = deserialize_res(string_res);

               //  sequence check
               if (res.seqnum == req.seqnum) {

                  res.perr = NONE;
                  return(res);

               } else {

                  res.perr = SEQ_ERR;
                  return(res);
               }

            } else {

               if (--retries_left == 0) {

                  // server seems to be offline, abandoning
                  expect_reply = false;
                  is_online = false;
                  break;

               } else {

                  // no response from server, retrying
                  // old socket will be confused; close it and open a new one
                  is_online = false;
                  connect();
                  // send request again, on new socket
                  zmq_client->send(msg_req, ZMQ_DONTWAIT);
               }
            }

         }  // end while(expect_reply)

      }  // end while(retries_left)

      // leave socket in coherent state...
      if(retries_left == 0)
         connect();

   } // if connected()

   res.verr = cvGenericError;
   res.perr = CONN_ERR;
   return(res);
}

bool V2NClient::read(uint32_t addr, uint32_t *data) {

   request_t req;
   response_t reply;

   req.op = READ;
   req.address = addr;
   req.data = 0;

   reply = access(req);
   *data = reply.data;

   last_perr = reply.perr;
   last_verr = reply.verr;

   // return true if VME access is ok...
   return((last_perr == 0) && (last_verr == 0));
}

bool V2NClient::write(uint32_t addr, uint32_t data) {

   request_t req;
   response_t reply;

   req.op = WRITE;
   req.address = addr;
   req.data = data;

   reply = access(req);

   last_perr = reply.perr;
   last_verr = reply.verr;

   // return true if VME access is ok...
   return((last_perr == 0) && (last_verr == 0));
}

ProtErrorCodes V2NClient::getperr(void) {

   return(last_perr);
}

CVErrorCodes V2NClient::getverr(void) {

   return(last_verr);
}

QString V2NClient::getperr_str(void) {

   return(ProtErrorCodes_str[last_perr]);
}

QString V2NClient::getverr_str(void) {

   return(CVErrorCodes_str[abs(last_verr)]);
}

ProtErrorCodes V2NClient::validate_param(vmeaccess_t op) {

   if( (op == READ) || (op == WRITE) )
      return NONE;
   else
      return OP_ERR;
}

ProtErrorCodes V2NClient::validate_param(CVAddressModifier am) {

   if( (am == cvA16_S) || (am == cvA16_U) || (am == cvA24_S_DATA) || (am == cvA24_U_DATA) || (am == cvA32_S_DATA) || (am == cvA32_U_DATA))
      return NONE;
   else
      return AM_ERR;
}

ProtErrorCodes V2NClient::validate_param(CVDataWidth dw) {

   if( (dw == cvD8) || (dw == cvD16) || (dw == cvD32) || (dw == cvD64))
      return NONE;
   else
      return DW_ERR;
}

void V2NClient::init(request_t &req) {

   req.op = (vmeaccess_t) 0;
   req.seqnum = 0;
   req.address = 0;
   req.data = 0;
   req.am = (CVAddressModifier) 0;
   req.dw = (CVDataWidth) 0;
   req.perr = (ProtErrorCodes) 0;
}

void V2NClient::init(response_t &res) {

   res.op = (vmeaccess_t) 0;
   res.seqnum = 0;
   res.verr = (CVErrorCodes) 0;
   res.data = 0;
   res.perr = (ProtErrorCodes) 0;
}

request_t V2NClient::deserialize_req(std::string string_req) {

   request_t req;
   int nconv;
   vmeaccess_t op;
   unsigned int seqnum;
   uint32_t address;
   uint32_t data;
   CVAddressModifier am = cvA32_U_DATA;
   CVDataWidth dw = cvD16;

   if((validate_param((vmeaccess_t) string_req[0]) != NONE)) {

      req.perr = OP_ERR;
      return(req);

   } else req.op = (vmeaccess_t) string_req[0];

   if(req.op == READ) {

      std::istringstream iss(string_req);
      std::string token;

      nconv = 0;
      while(getline(iss, token, '|')) {

         std::istringstream ss(token);

         if(nconv == 0) op = (vmeaccess_t) token[0];
         else if(nconv == 1) ss >> seqnum;
         else if(nconv == 2) ss >> address;
         else if(nconv == 3) am = (CVAddressModifier) atoi(token.c_str());
         else if(nconv == 4) dw = (CVDataWidth) atoi(token.c_str());

         nconv++;
      }

      if(nconv < 5)
         req.perr = LOW_PARAM;
      else if(nconv > 5)
         req.perr = HIGH_PARAM;
      else {	// nconv == 5

         // parameters range validation
         if(validate_param(am) != NONE)
            req.perr = AM_ERR;
         else if(validate_param(dw) != NONE)
            req.perr = DW_ERR;
         else {

            req.seqnum = seqnum;
            req.address = address;
            req.am = am;
            req.dw = dw;
            req.perr = NONE;
         }
      }

   } else if(req.op == WRITE) {

      std::istringstream iss(string_req);
      std::string token;

      nconv = 0;
      while(getline(iss, token, '|')) {

         std::istringstream ss(token);

         if(nconv == 0) op = (vmeaccess_t) token[0];
         else if(nconv == 1) ss >> seqnum;
         else if(nconv == 2) ss >> address;
         else if(nconv == 3) ss >> data;
         else if(nconv == 4) am = (CVAddressModifier) atoi(token.c_str());
         else if(nconv == 5) dw = (CVDataWidth) atoi(token.c_str());

         nconv++;
      }

      if(nconv < 6)
         req.perr = LOW_PARAM;
      else if(nconv > 6)
         req.perr = HIGH_PARAM;
      else {    // nconv == 6

         // parameters range validation
         if(validate_param(am) != NONE)
            req.perr = AM_ERR;
         else if(validate_param(dw) != NONE)
            req.perr = DW_ERR;
         else {

            req.seqnum = seqnum;
            req.address = address;
            req.data = data;
            req.am = am;
            req.dw = dw;
            req.perr = NONE;
         }
      }
   }

   return(req);
}

response_t V2NClient::deserialize_res(std::string string_res) {

   response_t res;
   int nconv = 0;
   vmeaccess_t op;
   unsigned int seqnum;
   CVErrorCodes verr = cvSuccess;
   uint32_t data;

   if((validate_param((vmeaccess_t) string_res[0]) != NONE)) {

      res.perr = OP_ERR;
      return(res);

   } else res.op = (vmeaccess_t) string_res[0];

   if(res.op == READ) {

      std::istringstream iss(string_res);
      std::string token;

      nconv = 0;
      while(getline(iss, token, '|')) {

         std::istringstream ss(token);

         if(nconv == 0) op = (vmeaccess_t) token[0];
         else if(nconv == 1) ss >> seqnum;
         else if(nconv == 2) verr = (CVErrorCodes) atoi(token.c_str());
         else if(nconv == 3) ss >> data;

         nconv++;
      }

      if(nconv < 4)
         res.perr = LOW_PARAM;
      else if(nconv > 4)
         res.perr = HIGH_PARAM;
      else {	// nconv == 4

         res.seqnum = seqnum;
         res.verr = verr;
         res.data = data;
         res.perr = NONE;
      }

   } else if(res.op == WRITE) {

      std::istringstream iss(string_res);
      std::string token;

      nconv = 0;
      while(getline(iss, token, '|')) {

         std::istringstream ss(token);

         if(nconv == 0) op = (vmeaccess_t) token[0];
         else if(nconv == 1) ss >> seqnum;
         else if(nconv == 2) verr = (CVErrorCodes) atoi(token.c_str());

         nconv++;
      }

      if(nconv < 3)
         res.perr = LOW_PARAM;
      else if(nconv > 3)
         res.perr = HIGH_PARAM;
      else {	// nconv == 3

         res.seqnum = seqnum;
         res.verr = verr;
         res.perr = NONE;
      }
   }

   return(res);
}

std::string V2NClient::serialize_req(request_t req) {

   std::stringstream sstr;

   if(req.op == WRITE)
      sstr << (char) req.op << "|" << req.seqnum << "|" << req.address << "|" << req.data << "|" << req.am << "|" << req.dw;
   else if(req.op == READ)
      sstr << (char) req.op << "|" << req.seqnum << "|" << req.address << "|" << req.am << "|" << req.dw;

   return(sstr.str());
}

std::string V2NClient::serialize_res(response_t res) {

   std::stringstream sstr;

   if(res.op == WRITE)
      sstr << (char) res.op << "|" << res.seqnum << "|" << res.verr;
   else if(res.op == READ)
      sstr << (char) res.op << "|" << res.seqnum << "|" << res.verr << "|" << res.data;

   return(sstr.str());
}

void V2NClient::debug(request_t req) {

   std::cout << "REQUEST: " << std::endl;
   std::cout << "  op     = " << (char) req.op << std::endl;
   std::cout << "  seqnum = " << req.seqnum << std::endl;
   std::cout << "  addr   = 0x" << std::hex << req.address << std::dec << std::endl;

   if(req.op == WRITE)
      std::cout << "  data   = " << req.data << std::endl;

   std::cout << "  am     = 0x" << std::hex << req.am << std::dec << std::endl;
   std::cout << "  dw     = 0x" << std::hex << req.dw << std::dec << std::endl;

   if( (req.perr > 0) && (req.perr < 7) )
      std::cout << "format error: " << ProtErrorCodes_str[req.perr] << std::endl;

   std::cout << std::endl;
}

void V2NClient::debug(response_t res) {

   std::cout << "RESPONSE: " << std::endl;
   std::cout << "  op     = " << (char) res.op << std::endl;
   std::cout << "  seqnum = " << res.seqnum << std::endl;
   if(res.op == READ)
      std::cout << "  data   = " << res.data << std::endl;
   std::cout << "VME error: " << res.verr << std::endl;

   if( (res.perr > 0) && (res.perr < 7) )
      std::cout << "format error: " << ProtErrorCodes_str[res.perr] << std::endl;

   std::cout << std::endl;
}
