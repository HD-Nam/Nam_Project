#ifndef __ACP_H
#define __ACP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "main.h"
#include "acp_type.h"
#include "crc.h"

#define ACP_FIN		0x01
#define ACP_SYN		0x02
#define ACP_RST		0x04
#define ACP_PSH		0x08
#define ACP_ACK		0x10
#define ACP_URG		0x20



acp *newAcp(uint16_t ip_addr, uint8_t maxConn, uint8_t maxPort, uint8_t maxSizePtk, uint16_t cycleTime);
Acp_ReturnType Acp_Listen(acp *acp_obj, uint8_t port);

void Acp_Process(acp *acp_obj, uint8_t Timecycle);
void acp_add_callback(acp_callback_t callback);
void Acp_pushMessage(acp *acp_obj, uint8_t *data, uint16_t length);
Acp_ReturnType Acp_pullMessageSend(acp *acp_obj, uint8_t *data, uint16_t *length);
acp_conn * Acp_Connect(acp *acp_obj, acp_point des, acp_point scr);
acp_conn * Acp_disconnect(acp_conn *connection);
Acp_ReturnType Acp_getConn(acp *acp_obj, uint8_t port, acp_conn *conn);
void Acp_EventDisconnected(acp *acp_obj, acp_notify_connected notify);
void Acp_EventEstablish(acp *acp_obj, acp_notify_connected notify);


void acp_sendPack(uint8_t *data, uint16_t length);
void acp_process(void);
void acp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
