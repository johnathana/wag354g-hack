/* ------------------------------------------------------------------------- */
/* i2c-algo-8260.h i2c driver algorithms for MPC8260 CPM		     */
/* ------------------------------------------------------------------------- */

/* $Id: i2c-algo-8260.h,v 1.1.1.2 2005/03/28 06:56:59 sparq Exp $ */

#ifndef I2C_ALGO_8260_H
#define I2C_ALGO_8260_H 1

#include <linux/i2c.h>

struct i2c_algo_8260_data {
	uint dp_addr;
	int reloc;
	volatile i2c8260_t *i2c;
	volatile iic_t	*iip;
	volatile cpm8260_t *cp;

	u_char	temp[513];
};

#endif /* I2C_ALGO_8260_H */
