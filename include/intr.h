#ifndef _INTR_H_
#define _INTR_H_

#define IOADR_MPIC_OCW2	0x0020
#define IOADR_MPIC_OCW2_BIT_MANUAL_EOI	0x60
#define IOADR_MPIC_ICW1	0x0020
#define IOADR_MPIC_ICW2	0x0021
#define IOADR_MPIC_ICW3	0x0021
#define IOADR_MPIC_ICW4	0x0021
#define IOADR_MPIC_OCW1	0x0021
#define IOADR_SPIC_ICW1	0x00a0
#define IOADR_SPIC_ICW2	0x00a1
#define IOADR_SPIC_ICW3	0x00a1
#define IOADR_SPIC_ICW4	0x00a1
#define IOADR_SPIC_OCW1	0x00a1

#define INTR_NUM_USER128	0x80

void intr_init(void);
void set_intr_desc(unsigned char intr_no, void *handler);
void intr_set_mask_master(unsigned char mask);
unsigned char intr_get_mask_master(void);
void intr_set_mask_slave(unsigned char mask);
unsigned char intr_get_mask_slave(void);

#endif /* _INTR_H_ */
