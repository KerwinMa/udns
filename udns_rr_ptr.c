/* $Id: udns_rr_ptr.c,v 1.11 2005/04/05 22:51:32 mjt Exp $
   parse/query PTR records

   Copyright (C) 2005  Michael Tokarev <mjt@corpit.ru>
   This file is part of UDNS library, an async DNS stub resolver.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library, in file named COPYING.LGPL; if not,
   write to the Free Software Foundation, Inc., 59 Temple Place,
   Suite 330, Boston, MA  02111-1307  USA

 */

#include <stdlib.h>
#include "udns.h"

int
dns_parse_ptr(const unsigned char *pkt, const unsigned char *pkte,
              void **result) {
  struct dns_rr_ptr *ret = NULL;
  struct dns_parse p;
  struct dns_rr rr;
  const unsigned char *cur;
  int r, l, c;
  char *sp;
  unsigned char ptr[DNS_MAXDN];

  /* first, validate the answer and count size of the result */
  l = c = 0;
  for(r = dns_firstrr(&p, &rr, DNS_C_IN, DNS_T_PTR, pkt, pkte);
      r > 0; r = dns_nextrr(&p, &rr)) {
    cur = rr.dnsrr_dptr;
    r = dns_getdn(pkt, &cur, rr.dnsrr_dend, ptr, sizeof(ptr));
    if (r <= 0 || cur != rr.dnsrr_dend)
      return DNS_E_PROTOCOL;
    l += dns_dntop_size(ptr);
    ++c;
  }
  if (r < 0)
    return DNS_E_PROTOCOL;
  if (!c)
    return DNS_E_NODATA;

  /* next, allocate and set up result */
  ret = malloc(sizeof(*ret) + sizeof(char **) * c + l + dns_stdrr_size(&p));
  if (!ret)
    return DNS_E_NOMEM;
  ret->dnsptr_nrr = c;
  ret->dnsptr_ptr = (char **)(ret+1);

  /* and 3rd, fill in result, finally */
  sp = (char*)(ret->dnsptr_ptr + c);
  c = 0;
  for(r = dns_firstrr(&p, &rr, DNS_C_IN, DNS_T_PTR, pkt, pkte);
      r > 0; r = dns_nextrr(&p, &rr)) {
    ret->dnsptr_ptr[c] = sp;
    cur = rr.dnsrr_dptr;
    dns_getdn(pkt, &cur, pkte, ptr, sizeof(ptr));
    sp += dns_dntop(ptr, sp, DNS_MAXNAME);
    ++c;
  }
  dns_stdrr_finish((struct dns_rr_null *)ret, sp, &p);
  *result = ret;
  return 0;
}

struct dns_query *
dns_submit_a4ptr(struct dns_ctx *ctx, const struct in_addr *addr,
                 dns_query_ptr_fn *cbck, void *data, time_t now) {
  unsigned char dn[DNS_A4RSIZE];
  dns_a4todn(addr, 0, dn, sizeof(dn));
  return
    dns_submit_dn(ctx, dn, DNS_C_IN, DNS_T_PTR, DNS_NOSRCH,
                  dns_parse_ptr, (dns_query_fn *)cbck, data, now);
}

struct dns_rr_ptr *
dns_resolve_a4ptr(struct dns_ctx *ctx, const struct in_addr *addr) {
  return (struct dns_rr_ptr *)
    dns_resolve(ctx, dns_submit_a4ptr(ctx, addr, NULL, NULL, 0));
}

struct dns_query *
dns_submit_a6ptr(struct dns_ctx *ctx, const struct in6_addr *addr,
                 dns_query_ptr_fn *cbck, void *data, time_t now) {
  unsigned char dn[DNS_A6RSIZE];
  dns_a6todn(addr, 0, dn, sizeof(dn));
  return
    dns_submit_dn(ctx, dn, DNS_C_IN, DNS_T_PTR, DNS_NOSRCH,
                  dns_parse_ptr, (dns_query_fn *)cbck, data, now);
}

struct dns_rr_ptr *
dns_resolve_a6ptr(struct dns_ctx *ctx, const struct in6_addr *addr) {
  return (struct dns_rr_ptr *)
    dns_resolve(ctx, dns_submit_a6ptr(ctx, addr, NULL, NULL, 0));
}
