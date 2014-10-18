#ifndef V2NCLIENT_H
#define V2NCLIENT_H

#define LINUX
#include <iostream>
#include <memory>
#include <stdint.h>
#include <stdlib.h>

#include <QString>
#include <QMutex>
#include "CAENVMEtypes.h"
#include "zmq.hpp"

// VME access type
typedef enum { READ = 'r', WRITE = 'w' } vmeaccess_t;
typedef enum { NONE = 0, LOW_PARAM, HIGH_PARAM, OP_ERR, AM_ERR, DW_ERR, SEQ_ERR, CONN_ERR } ProtErrorCodes;

char const* const ProtErrorCodes_str[] = { "none", "missing parameters", "too many parameters", "opcode error", "address modifier error", "data width error" , "sequence error", "connection error" };
char const* const CVErrorCodes_str[] = { "success", "bus error", "communication error", "generic error", "invalid param", "timeout error" };

//  Provide random number from 0..(num-1)
#define within(num) (int) ((float) (num) * rand() / (RAND_MAX + 1.0))

// REQUEST:  vmeaccess_t op = WRITE | unsigned char seqnum | uint32_t address | uint32_t data | CVAddressModifier am | CVDataWidth dw
// REQUEST:  vmeaccess_t op = READ  | unsigned char seqnum | uint32_t address | CVAddressModifier am | CVDataWidth dw

struct _req {

   vmeaccess_t op;
   unsigned int seqnum;
   uint32_t address;
   uint32_t data;
   CVAddressModifier am;
   CVDataWidth dw;

   ProtErrorCodes perr;
};

// REPLY:    vmeaccess_t op = WRITE | unsigned char seqnum | CVErrorCodes verr
// REPLY:    vmeaccess_t op = READ  | unsigned char seqnum | CVErrorCodes verr | uint32_t data

struct _res {

   vmeaccess_t op;
   unsigned int seqnum;
   CVErrorCodes verr;
   uint32_t data;

   ProtErrorCodes perr;
};

typedef struct _req request_t;
typedef struct _res response_t;

class V2NClient {

private:

   QString hostname;
   uint16_t port;
   bool is_online;

   zmq::context_t *zmq_ctx;
   zmq::socket_t *zmq_client;
   std::auto_ptr<zmq::socket_t> sockptr;

   zmq::error_t last_zmqerr;
   ProtErrorCodes last_perr;
   CVErrorCodes last_verr;

   ProtErrorCodes validate_param(vmeaccess_t op);
   ProtErrorCodes validate_param(CVAddressModifier am);
   ProtErrorCodes validate_param(CVDataWidth dw);

   void init(request_t &req);
   void init(response_t &res);

   request_t deserialize_req(std::string string_req);
   response_t deserialize_res(std::string string_res);

   std::string serialize_req(request_t req);
   std::string serialize_res(response_t res);

   void debug(request_t req);
   void debug(response_t res);

public:

   V2NClient();

   void setup(QString _hostname, uint16_t _port);

   QString gethostname(void);
   uint16_t getport(void);

   bool connect(void);
   bool disconnect(void);
   void close(void);
   bool online(void);

   response_t access(request_t);
   bool read(uint32_t addr, uint32_t *data);
   bool write(uint32_t addr, uint32_t data);

   ProtErrorCodes getperr(void);
   CVErrorCodes getverr(void);

   QString getperr_str(void);
   QString getverr_str(void);
};

#endif // V2NCLIENT_H
