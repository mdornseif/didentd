#include "uint16.h"
#include "open.h"
#include "tai.h"
#include "cdb.h"
#include "byte.h"
#include "case.h"
#include "dns.h"
#include "seek.h"
#include "response.h"

static int want(char *owner,char type[2])
{
  unsigned int pos;
  static char *d;
  char x[10];
  uint16 datalen;

  pos = dns_packet_skipname(response,response_len,12); if (!pos) return 0;
  pos += 4;

  while (pos < response_len) {
    pos = dns_packet_getname(response,response_len,pos,&d); if (!pos) return 0;
    pos = dns_packet_copy(response,response_len,pos,x,10); if (!pos) return 0;
    if (dns_domain_equal(d,owner))
      if (byte_equal(type,2,x))
        return 0;
    uint16_unpack_big(x + 8,&datalen);
    pos += datalen;
  }
  return 1;
}

static char *d1;

static struct tai now;
static struct cdb c;
static char data[512];
static uint32 dlen;
static unsigned int dpos;
static char type[2];
static char ttl[4];

static int find(char *d,int flagwild)
{
  int r;
  char ch;
  struct tai cutoff;
  char ttd[8];
  double newttl;

  for (;;) {
    r = cdb_findnext(&c,d,dns_domain_length(d));
    if (r <= 0) return r;
    dlen = cdb_datalen(&c);
    if (dlen > sizeof data) return -1;
    if (cdb_read(&c,data,dlen,cdb_datapos(&c)) == -1) return -1;
    dpos = dns_packet_copy(data,dlen,0,type,2); if (!dpos) return -1;
    dpos = dns_packet_copy(data,dlen,dpos,&ch,1); if (!dpos) return -1;
    if (flagwild != (ch == '*')) continue;
    dpos = dns_packet_copy(data,dlen,dpos,ttl,4); if (!dpos) return -1;
    dpos = dns_packet_copy(data,dlen,dpos,ttd,8); if (!dpos) return -1;
    if (byte_diff(ttd,8,"\0\0\0\0\0\0\0\0")) {
      tai_unpack(ttd,&cutoff);
      if (byte_equal(ttl,4,"\0\0\0\0")) {
	if (tai_less(&cutoff,&now)) continue;
	tai_sub(&cutoff,&cutoff,&now);
	newttl = tai_approx(&cutoff);
	if (newttl <= 2.0) newttl = 2.0;
	if (newttl >= 3600.0) newttl = 3600.0;
	uint32_pack_big(ttl,(uint32) newttl);
      }
      else
	if (!tai_less(&cutoff,&now)) continue;
    }
    return 1;
  }
}

static int dobytes(unsigned int len)
{
  char buf[20];
  if (len > 20) return 0;
  dpos = dns_packet_copy(data,dlen,dpos,buf,len);
  if (!dpos) return 0;
  return response_addbytes(buf,len);
}

static int doname(void)
{
  dpos = dns_packet_getname(data,dlen,dpos,&d1);
  if (!dpos) return 0;
  return response_addname(d1);
}

static int doit(char *q,char qtype[2])
{
  unsigned int bpos;
  unsigned int anpos;
  unsigned int aupos;
  unsigned int arpos;
  char *control;
  char *wild;
  int flaggavesoa;
  int flagfound;
  int r;
  int flagns;
  int flagauthoritative;
  char x[20];
  uint16 u16;

  anpos = response_len;

  control = q;
  for (;;) {
    flagns = 0;
    flagauthoritative = 0;
    cdb_findstart(&c);
    while (r = find(control,0)) {
      if (r == -1) return 0;
      if (byte_equal(type,2,DNS_T_SOA)) flagauthoritative = 1;
      if (byte_equal(type,2,DNS_T_NS)) flagns = 1;
    }
    if (flagns) break;
    if (!*control) return 0; /* q is not within our bailiwick */
    control += *control;
    control += 1;
  }

  if (!flagauthoritative) {
    response[2] &= ~4;
    goto AUTHORITY; /* q is in a child zone */
  }


  flaggavesoa = 0;
  flagfound = 0;
  wild = q;

  for (;;) {
    cdb_findstart(&c);
    while (r = find(wild,wild != q)) {
      if (r == -1) return 0;
      flagfound = 1;
      if (flaggavesoa && byte_equal(type,2,DNS_T_SOA)) continue;
      if (byte_diff(type,2,qtype) && byte_diff(qtype,2,DNS_T_ANY) && byte_diff(type,2,DNS_T_CNAME)) continue;
      if (!response_rstart(q,type,ttl)) return 0;
      if (byte_equal(type,2,DNS_T_NS) || byte_equal(type,2,DNS_T_CNAME) || byte_equal(type,2,DNS_T_PTR)) {
	if (!doname()) return 0;
      }
      else if (byte_equal(type,2,DNS_T_MX)) {
	if (!dobytes(2)) return 0;
	if (!doname()) return 0;
      }
      else if (byte_equal(type,2,DNS_T_SOA)) {
	if (!doname()) return 0;
	if (!doname()) return 0;
	if (!dobytes(20)) return 0;
        flaggavesoa = 1;
      }
      else
        if (!response_addbytes(data + dpos,dlen - dpos)) return 0;
      response_rfinish(RESPONSE_ANSWER);
    }
    if (flagfound) break;
    if (wild == control) break;
    if (!*wild) break; /* impossible */
    wild += *wild;
    wild += 1;
  }

  if (!flagfound)
    response_nxdomain();


  AUTHORITY:
  aupos = response_len;

  if (flagauthoritative && (aupos == anpos)) {
    cdb_findstart(&c);
    while (r = find(control,0)) {
      if (r == -1) return 0;
      if (byte_equal(type,2,DNS_T_SOA)) {
        if (!response_rstart(control,DNS_T_SOA,ttl)) return 0;
	if (!doname()) return 0;
	if (!doname()) return 0;
	if (!dobytes(20)) return 0;
        response_rfinish(RESPONSE_AUTHORITY);
        break;
      }
    }
  }
  else
    if (want(control,DNS_T_NS)) {
      cdb_findstart(&c);
      while (r = find(control,0)) {
        if (r == -1) return 0;
        if (byte_equal(type,2,DNS_T_NS)) {
          if (!response_rstart(control,DNS_T_NS,ttl)) return 0;
	  if (!doname()) return 0;
          response_rfinish(RESPONSE_AUTHORITY);
        }
      }
    }

  arpos = response_len;

  bpos = anpos;
  while (bpos < arpos) {
    bpos = dns_packet_skipname(response,arpos,bpos); if (!bpos) return 0;
    bpos = dns_packet_copy(response,arpos,bpos,x,10); if (!bpos) return 0;
    if (byte_equal(x,2,DNS_T_NS) || byte_equal(x,2,DNS_T_MX)) {
      if (byte_equal(x,2,DNS_T_NS)) {
        if (!dns_packet_getname(response,arpos,bpos,&d1)) return 0;
      }
      else
        if (!dns_packet_getname(response,arpos,bpos + 2,&d1)) return 0;
      case_lowerb(d1,dns_domain_length(d1));
      if (want(d1,DNS_T_A)) {
	cdb_findstart(&c);
	while (r = find(d1,0)) {
          if (r == -1) return 0;
	  if (byte_equal(type,2,DNS_T_A)) {
            if (!response_rstart(d1,DNS_T_A,ttl)) return 0;
	    if (!dobytes(4)) return 0;
            response_rfinish(RESPONSE_ADDITIONAL);
	  }
        }
      }
    }
    uint16_unpack_big(x + 8,&u16);
    bpos += u16;
  }

  if (flagauthoritative && (response_len > 512)) {
    byte_zero(response + RESPONSE_ADDITIONAL,2);
    response_len = arpos;
    if (response_len > 512) {
      byte_zero(response + RESPONSE_AUTHORITY,2);
      response_len = aupos;
    }
  }

  return 1;
}

int respond(char *q,char qtype[2],char ip[4])
{
  int fd;
  int result;

  tai_now(&now);
  fd = open_read("data.cdb");
  if (fd == -1) return 0;
  cdb_init(&c,fd);
  result = doit(q,qtype);
  cdb_free(&c);
  close(fd);
  return result;
}
