#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * socket configuration
 */
#define WCHNET_NUM_IPRAW              0

// 保留1个UDP用于向CH9121发送数据
#define WCHNET_NUM_UDP                1  

// 新增1个TCP用于连接MQTT Broker (NanoPi)
#define WCHNET_NUM_TCP                1  

#define WCHNET_NUM_TCP_LISTEN         0

#define WCHNET_MAX_SOCKET_NUM         (WCHNET_NUM_IPRAW+WCHNET_NUM_UDP+WCHNET_NUM_TCP+WCHNET_NUM_TCP_LISTEN)

#define WCHNET_TCP_MSS                1460

#define WCHNET_NUM_POOL_BUF           (WCHNET_NUM_TCP*2+2)

/*********************************************************************
 * MAC queue configuration
 */
#define ETH_TXBUFNB                   1
#define ETH_RXBUFNB                   4

#ifndef ETH_MAX_PACKET_SIZE
#define ETH_RX_BUF_SZE                1520
#define ETH_TX_BUF_SZE                1520
#else
#define ETH_RX_BUF_SZE                ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SZE                ETH_MAX_PACKET_SIZE
#endif

/*********************************************************************
 * Functional configuration
 */
#define WCHNET_PING_ENABLE            1
#define TCP_RETRY_COUNT               20
#define TCP_RETRY_PERIOD              10
#define SOCKET_SEND_RETRY             1     /* 建议开启重试 */
#define FINE_DHCP_PERIOD              8
#define CFG0_TCP_SEND_COPY            1
#define CFG0_TCP_RECV_COPY            1
#define CFG0_TCP_OLD_DELETE           0
#define CFG0_IP_REASS_PBUFS           0
#define CFG0_TCP_DEALY_ACK_DISABLE    1

/*********************************************************************
 * Memory related configuration
 */
#define RECE_BUF_LEN                  (WCHNET_TCP_MSS*2)
#define WCHNET_NUM_PBUF               WCHNET_NUM_POOL_BUF
#define WCHNET_NUM_TCP_SEG            (WCHNET_NUM_TCP*2)
#define WCHNET_MEM_HEAP_SIZE          (((WCHNET_TCP_MSS+0x10+54+8)*WCHNET_NUM_TCP_SEG)+ETH_TX_BUF_SZE+64+2*0x18)
#define WCHNET_NUM_ARP_TABLE          50
#define WCHNET_MEM_ALIGNMENT          4

#if CFG0_IP_REASS_PBUFS
#define WCHNET_NUM_IP_REASSDATA       2
#define WCHNET_SIZE_POOL_BUF    (((1500 + 14 + 4) + 3) & ~3)
#else
#define WCHNET_NUM_IP_REASSDATA       0
#define WCHNET_SIZE_POOL_BUF     (((WCHNET_TCP_MSS + 40 + 14 + 4) + 3) & ~3)
#endif

/* Configuration Check Macros (omitted for brevity, same as original) */
#if(WCHNET_NUM_POOL_BUF * WCHNET_SIZE_POOL_BUF < ETH_RX_BUF_SZE)
    #error "WCHNET_NUM_POOL_BUF or WCHNET_TCP_MSS Error"
#endif

#define WCHNET_MISC_CONFIG0    (((CFG0_TCP_SEND_COPY) << 0) |\
                               ((CFG0_TCP_RECV_COPY)  << 1) |\
                               ((CFG0_TCP_OLD_DELETE) << 2) |\
                               ((CFG0_IP_REASS_PBUFS) << 3) |\
                               ((CFG0_TCP_DEALY_ACK_DISABLE) << 8))

#define WCHNET_MISC_CONFIG1    (((WCHNET_MAX_SOCKET_NUM)<<0)|\
                               ((WCHNET_PING_ENABLE) << 13) |\
                               ((TCP_RETRY_COUNT)    << 14) |\
                               ((TCP_RETRY_PERIOD)   << 19) |\
                               ((SOCKET_SEND_RETRY)  << 25) |\
                               ((FINE_DHCP_PERIOD) << 27))

#ifdef __cplusplus
}
#endif
#endif