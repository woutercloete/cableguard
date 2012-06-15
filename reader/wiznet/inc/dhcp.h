/**
 * @file		dhcp.c
 * @brief 		waiting functions
 */

#ifndef _DHCP_H_
#define _DHCP_H_


/* DHCP state machine. */
#define	STATE_DHCP_DISCOVER	1
#define	STATE_DHCP_REQUEST	2
#define	STATE_DHCP_LEASED	3
#define	STATE_DHCP_REREQUEST	4
#define	STATE_DHCP_RELEASE	5

#define	MAX_DHCP_RETRY		3
#define	DHCP_WAIT_TIME		5

#define DHCP_FLAGSBROADCAST	0x8000

/* UDP port numbers for DHCP */
#define	DHCP_SERVER_PORT	67	/* from server to client */
#define DHCP_CLIENT_PORT	68	/* from client to server */

/* DHCP message OP code */
#define DHCP_BOOTREQUEST	1
#define DHCP_BOOTREPLY		2

/* DHCP message type */
#define	DHCP_DISCOVER		1
#define DHCP_OFFER		2
#define	DHCP_REQUEST		3
#define	DHCP_DECLINE		4
#define	DHCP_ACK		5
#define DHCP_NAK		6
#define	DHCP_RELEASE		7
#define DHCP_INFORM		8

/* DHCP RETRANSMISSION TIMEOUT (microseconds) */
#define DHCP_INITIAL_RTO	( 4*1000000)
#define DHCP_MAX_RTO		(64*1000000)

#define DHCP_HTYPE10MB		1
#define DHCP_HTYPE100MB		2

#define DHCP_HLENETHERNET	6
#define DHCP_HOPS		0
#define DHCP_SECS		0

#define MAGIC_COOKIE		0x63825363

#define DEFAULT_LEASETIME	0xffffffff	/**< infinite lease time */

/**
 * @brief	DHCP option and value (cf. RFC1533)
 */
enum
{
	padOption		=	0,
	subnetMask		=	1,
	timerOffset		=	2,
	routersOnSubnet		=	3,
	timeServer		=	4,
	nameServer		=	5,
	dns			=	6,
	logServer		=	7,
	cookieServer		=	8,
	lprServer		=	9,
	impressServer		=	10,
	resourceLocationServer	=	11,
	hostName		=	12,
	bootFileSize		=	13,
	meritDumpFile		=	14,
	domainName		=	15,
	swapServer		=	16,
	rootPath		=	17,
	extentionsPath		=	18,
	IPforwarding		=	19,
	nonLocalSourceRouting	=	20,
	policyFilter		=	21,
	maxDgramReasmSize	=	22,
	defaultIPTTL		=	23,
	pathMTUagingTimeout	=	24,
	pathMTUplateauTable	=	25,
	ifMTU			=	26,
	allSubnetsLocal		=	27,
	broadcastAddr		=	28,
	performMaskDiscovery	=	29,
	maskSupplier		=	30,
	performRouterDiscovery	=	31,
	routerSolicitationAddr	=	32,
	staticRoute		=	33,
	trailerEncapsulation	=	34,
	arpCacheTimeout		=	35,
	ethernetEncapsulation	=	36,
	tcpDefaultTTL		=	37,
	tcpKeepaliveInterval	=	38,
	tcpKeepaliveGarbage	=	39,
	nisDomainName		=	40,
	nisServers		=	41,
	ntpServers		=	42,
	vendorSpecificInfo	=	43,
	netBIOSnameServer	=	44,
	netBIOSdgramDistServer	=	45,
	netBIOSnodeType		=	46,
	netBIOSscope		=	47,
	xFontServer		=	48,
	xDisplayManager		=	49,
	dhcpRequestedIPaddr	=	50,
	dhcpIPaddrLeaseTime	=	51,
	dhcpOptionOverload	=	52,
	dhcpMessageType		=	53,
	dhcpServerIdentifier	=	54,
	dhcpParamRequest	=	55,
	dhcpMsg			=	56,
	dhcpMaxMsgSize		=	57,
	dhcpT1value		=	58,
	dhcpT2value		=	59,
	dhcpClassIdentifier	=	60,
	dhcpClientIdentifier	=	61,
	endOption		=	255
};


/**
 * @brief		for the DHCP message
 */
typedef struct _RIP_MSG
{
	u_char	op;
	u_char	htype;
	u_char	hlen;
	u_char	hops;
	u_long	xid;
	u_int	secs;
	u_int	flags;
	u_char	ciaddr[4];
	u_char	yiaddr[4];
	u_char	siaddr[4];
	u_char	giaddr[4];
	u_char	chaddr[16];
	u_char	sname[64];
	u_char	file[128];
	u_char	OPT[312];
}RIP_MSG;

#define MAX_DHCP_OPT	16


extern void init_dhcp_client(u_char* mac, SOCKET s, void (*ip_update)(void),void (*ip_conflict)(void));	// Initialize the DHCP client
extern u_int init_dhcpc_ch(SOCKET s, u_char* mac);								// Initialize the socket for DHCP client
extern u_int getIP_DHCPS(u_char* mac);								// Get the network configuration from the DHCP server
extern void check_DHCP_state(SOCKET s, u_char* mac);							// Check the DHCP state

#endif	/* _DHCP_H_ */
