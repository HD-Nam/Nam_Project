#ifndef __LO_PTC_H__
#define __LO_PTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "lo_ptc_type.h"
#include "crc.h"

lcp_t *newLcp(uint16_t ip_addr, uint8_t maxConn, uint8_t maxPort, uint8_t maxSizePtk, uint8_t cycleTime);
void Lcp_pushMessage(lcp_t *lcp_obj, uint8_t *data, uint16_t length);
uint8_t Lcp_pullMessage(lcp_t *lcp_obj, uint8_t *data, uint16_t *length);
uint8_t Lcp_pullMessageSend(lcp_t *lcp_obj, uint8_t *data, uint16_t *length);
uint8_t Lcp_Listen(lcp_t *lcp_obj, uint8_t port);
lcp_conn_t * Lcp_Connect(lcp_t *lcp_obj, point des);
void Lcp_EventEstablish(lcp_t *lcp_obj, lcp_notify_connected notify);
void Lcp_Process(lcp_t *lcp_obj, uint8_t Timecycle);
void Lcp_nProcess(lcp_t *lcp_obj, uint8_t Timecycle);
void Lcp_Runtime(lcp_t *lcp_obj, uint16_t timecircle);

void Lcp_EventEstablish(lcp_t *lcp_obj, lcp_notify_connected notify);
void Lcp_EventSendMessage(lcp_t *lcp_obj, lcp_notify_connected notify);
void Lcp_EventDisconnected(lcp_t *lcp_obj, lcp_notify_connected notify);

#ifdef __cplusplus
}
#endif
#endif
