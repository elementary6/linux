/*
 * Copyright (c) 2010 Broadcom Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef BRCMFMAC_DEBUG_H
#define BRCMFMAC_DEBUG_H

#include <linux/net.h>	/* net_ratelimit() */

/* message levels */
#define BRCMF_TRACE_VAL		0x00000002
#define BRCMF_INFO_VAL		0x00000004
#define BRCMF_DATA_VAL		0x00000008
#define BRCMF_CTL_VAL		0x00000010
#define BRCMF_TIMER_VAL		0x00000020
#define BRCMF_HDRS_VAL		0x00000040
#define BRCMF_BYTES_VAL		0x00000080
#define BRCMF_INTR_VAL		0x00000100
#define BRCMF_GLOM_VAL		0x00000200
#define BRCMF_EVENT_VAL		0x00000400
#define BRCMF_BTA_VAL		0x00000800
#define BRCMF_FIL_VAL		0x00001000
#define BRCMF_USB_VAL		0x00002000
#define BRCMF_SCAN_VAL		0x00004000
#define BRCMF_CONN_VAL		0x00008000
#define BRCMF_BCDC_VAL		0x00010000
#define BRCMF_SDIO_VAL		0x00020000
#define BRCMF_MSGBUF_VAL	0x00040000
#define BRCMF_PCIE_VAL		0x00080000
#define BRCMF_FWCON_VAL		0x00100000
#define BRCMF_ULP_VAL		0x00200000

/* set default print format */
#undef pr_fmt
#define pr_fmt(fmt)		KBUILD_MODNAME ": " fmt

__printf(2, 3)
void __brcmf_err(const char *func, const char *fmt, ...);
/* Macro for error messages. When debugging / tracing the driver all error
 * messages are important to us.
 */
#define brcmf_err(fmt, ...)						\
	do {								\
		if (IS_ENABLED(CONFIG_BRCMDBG) ||			\
		    IS_ENABLED(CONFIG_BRCM_TRACING) ||			\
		    net_ratelimit())					\
			__brcmf_err(__func__, fmt, ##__VA_ARGS__);	\
	} while (0)

#if defined(DEBUG) || defined(CONFIG_BRCM_TRACING)

/* For debug/tracing purposes treat info messages as errors */
#define brcmf_info brcmf_err

__printf(3, 4)
void __brcmf_dbg(u32 level, const char *func, const char *fmt, ...);
#define brcmf_dbg(level, fmt, ...)				\
do {								\
	__brcmf_dbg(BRCMF_##level##_VAL, __func__,		\
		    fmt, ##__VA_ARGS__);			\
} while (0)
#define BRCMF_DATA_ON()		(brcmf_msg_level & BRCMF_DATA_VAL)
#define BRCMF_CTL_ON()		(brcmf_msg_level & BRCMF_CTL_VAL)
#define BRCMF_HDRS_ON()		(brcmf_msg_level & BRCMF_HDRS_VAL)
#define BRCMF_BYTES_ON()	(brcmf_msg_level & BRCMF_BYTES_VAL)
#define BRCMF_GLOM_ON()		(brcmf_msg_level & BRCMF_GLOM_VAL)
#define BRCMF_EVENT_ON()	(brcmf_msg_level & BRCMF_EVENT_VAL)
#define BRCMF_FIL_ON()		(brcmf_msg_level & BRCMF_FIL_VAL)
#define BRCMF_FWCON_ON()	(brcmf_msg_level & BRCMF_FWCON_VAL)
#define BRCMF_SCAN_ON()		(brcmf_msg_level & BRCMF_SCAN_VAL)

#else /* defined(DEBUG) || defined(CONFIG_BRCM_TRACING) */

#define brcmf_info(fmt, ...)						\
	do {								\
		pr_info("%s: " fmt, __func__, ##__VA_ARGS__);		\
	} while (0)

#define brcmf_dbg(level, fmt, ...) no_printk(fmt, ##__VA_ARGS__)

#define BRCMF_DATA_ON()		0
#define BRCMF_CTL_ON()		0
#define BRCMF_HDRS_ON()		0
#define BRCMF_BYTES_ON()	0
#define BRCMF_GLOM_ON()		0
#define BRCMF_EVENT_ON()	0
#define BRCMF_FIL_ON()		0
#define BRCMF_FWCON_ON()	0
#define BRCMF_SCAN_ON()		0

#endif /* defined(DEBUG) || defined(CONFIG_BRCM_TRACING) */

#define brcmf_dbg_hex_dump(test, data, len, fmt, ...)			\
do {									\
	trace_brcmf_hexdump((void *)data, len);				\
	if (test)							\
		brcmu_dbg_hex_dump(data, len, fmt, ##__VA_ARGS__);	\
} while (0)

extern int brcmf_msg_level;

struct brcmf_bus;
struct brcmf_pub;
#ifdef DEBUG
void brcmf_debugfs_init(void);
void brcmf_debugfs_exit(void);
int brcmf_debug_attach(struct brcmf_pub *drvr);
void brcmf_debug_detach(struct brcmf_pub *drvr);
struct dentry *brcmf_debugfs_get_devdir(struct brcmf_pub *drvr);
int brcmf_debugfs_add_entry(struct brcmf_pub *drvr, const char *fn,
			    int (*read_fn)(struct seq_file *seq, void *data));
int brcmf_debug_create_memdump(struct brcmf_bus *bus, const void *data,
			       size_t len);
#else
static inline void brcmf_debugfs_init(void)
{
}
static inline void brcmf_debugfs_exit(void)
{
}
static inline int brcmf_debug_attach(struct brcmf_pub *drvr)
{
	return 0;
}
static inline void brcmf_debug_detach(struct brcmf_pub *drvr)
{
}
static inline
int brcmf_debugfs_add_entry(struct brcmf_pub *drvr, const char *fn,
			    int (*read_fn)(struct seq_file *seq, void *data))
{
	return 0;
}
static inline
int brcmf_debug_create_memdump(struct brcmf_bus *bus, const void *data,
			       size_t len)
{
	return 0;
}
#endif

#endif /* BRCMFMAC_DEBUG_H */
