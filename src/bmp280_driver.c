#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/types.h>

/* ************************************************************************** */
/*                              Module Constants                              */
/* ************************************************************************** */

#define DEVICE_ADDRESS 0x76

/* Private device structure */
struct mse_dev
{
	struct i2c_client *client;
	struct miscdevice mse_miscdevice;
	char name[9]; /* msedrvXX */
};

static struct {
	union {
		struct {
			uint8_t len;
			uint8_t reg;
			uint8_t val;
		};
		unsigned long raw;
	};
} __bmp280_args;

/*
 * Definicion de los ID correspondientes al Device Tree.
 */

static const struct of_device_id mse_dt_ids[] =
{
    { .compatible = "imd,mse_driver", },
    { /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, mse_dt_ids);

// ---------------------------------------------------------------------------------

static int bmp280_read_reg(struct i2c_adapter * __i2c_adapter, uint8_t reg, uint8_t nbytes)
{
    static int out;
    static struct i2c_msg msg[2];

	msg[0].addr = DEVICE_ADDRESS;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &reg;

	msg[1].addr = DEVICE_ADDRESS;
	msg[1].flags = I2C_M_RD | I2C_M_RECV_LEN;
	msg[1].len = 1;
	msg[1].buf = (unsigned char *)&out;
    
    i2c_transfer(__i2c_adapter, msg, 2);

    return out;
}

static int bmp280_write_reg(struct i2c_client * __i2c_client, uint8_t reg, int value, uint8_t nbytes)
{
	//static struct i2c_msg msg[2];
	
	//msg[0].addr = DEVICE_ADDRESS;
	//msg[0].flags = 0;
	//msg[0].len = 1;
	//msg[0].buf = &reg;
	
	//msg[1].addr = DEVICE_ADDRESS;
	//msg[1].flags = 0;
	//msg[1].len = 1;
	//msg[1].buf = (unsigned char *)&value;

	i2c_smbus_write_byte_data(__i2c_client, reg, value);
	//i2c_transfer(__i2c_adapter, msg, 2);

    return 0;
}

/* User is reading data from /dev/msedrvXX */
static ssize_t mse_read(struct file *file, char __user *userbuf, size_t count, loff_t *offset)
{
	struct mse_dev *mse;
    	static struct i2c_msg msg[2];
    	static char out;
    	unsigned char reg;

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    if (copy_from_user(&reg, userbuf, 1)) {
        return -EFAULT;
    }

	msg[0].addr = DEVICE_ADDRESS;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &reg;

	msg[1].addr = DEVICE_ADDRESS;
	msg[1].flags = I2C_M_RD | I2C_M_RECV_LEN;
	msg[1].len = 1;
	msg[1].buf = (unsigned char *)&out;

    i2c_transfer(mse->client->adapter, msg, 2);

	// Copiar los datos leídos al espacio de usuario
    if (copy_to_user(userbuf, &out, count)) {
        // Error al copiar datos al espacio de usuario
        return -EFAULT;
    }

    return count;
}

static ssize_t mse_write(struct file *file, const char __user *userbuf, size_t len, loff_t *offset)
{
    struct mse_dev *mse;

	unsigned char reg;
	unsigned char data;

    if (copy_from_user(&reg, userbuf, 1)) {
        return -EFAULT;
    }
    
    if (copy_from_user(&data, userbuf + 1, 1)) {
        return -EFAULT;
    }

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

	i2c_smbus_write_byte_data(mse->client, reg, data);

	//i2c_transfer(mse->client->adapter, msg, 2);


    return 0;
}

static long mse_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;
    struct mse_dev *mse;

    __bmp280_args.raw = arg;

    //pr_info("mse_ioctl() fue invocada. cmd = %d, arg = %lx\n", cmd, arg);   // debug
    pr_info("BMP280 args received: len %x; reg %x, val %x", __bmp280_args.len, __bmp280_args.reg, __bmp280_args.val);

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    

	switch (cmd) {
        case 0:             // read
            ret = bmp280_read_reg(mse->client->adapter, __bmp280_args.reg, __bmp280_args.len);
            break;
    
        case 1:             // write
            bmp280_write_reg(mse->client, __bmp280_args.reg, __bmp280_args.val, __bmp280_args.len);
            break;

        default:
            break;
    }


    return ret;
}


/* declaracion de una estructura del tipo file_operations */

static const struct file_operations mse_fops =
{
    .owner = THIS_MODULE,
    .read = mse_read,
    .write = mse_write,
    .unlocked_ioctl = mse_ioctl,
};

/*--------------------------------------------------------------------------------*/
static int mse_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct mse_dev *mse;
    static int counter = 0;
    int ret_val;

    /* Allocate new private structure */
    mse = devm_kzalloc(&client->dev, sizeof(struct mse_dev), GFP_KERNEL);

    /* Store pointer to the device-structure in bus device context */
    i2c_set_clientdata(client,mse);

	/* Store pointer to I2C client device in the private structure */
	mse->client = client;

    /* Initialize the misc device, mse is incremented after each probe call */
    sprintf(mse->name, "mse%02d", counter++);

    mse->mse_miscdevice.name = mse->name;
    mse->mse_miscdevice.minor = MISC_DYNAMIC_MINOR;
    mse->mse_miscdevice.fops = &mse_fops;

    /* Register misc device */
    ret_val = misc_register(&mse->mse_miscdevice);

    if (ret_val != 0)
    {
        pr_err("No se pudo registrar el dispositivo %s\n", mse->mse_miscdevice.name);
        return ret_val;
    }

    pr_info("Dispositivo %s: minor asignado: %i\n", mse->mse_miscdevice.name, mse->mse_miscdevice.minor);

    return 0;
}

static void mse_remove(struct i2c_client * client)
{
    struct mse_dev * mse;

    /* Get device structure from bus device context */
    mse = i2c_get_clientdata(client);

    /* Deregister misc device */
    misc_deregister(&mse->mse_miscdevice);
}

/*--------------------------------------------------------------------------------*/

static struct i2c_driver mse_driver =
{
    .probe= mse_probe,
    .remove= mse_remove,
    .driver =
    {
        .name = "mse_driver",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(mse_dt_ids),
    },
};

/*----------------------------------------------------------------------*/



/**********************************************************************
 * Esta seccion define cuales funciones seran las ejecutadas al cargar o
 * remover el modulo respectivamente. Es hecho implicitamente,
 * termina declarando init() exit()
 **********************************************************************/
module_i2c_driver(mse_driver);

/**********************************************************************
 * Seccion sobre Informacion del modulo
 **********************************************************************/
MODULE_AUTHOR("Lucas Constantino <constantino.lucas12@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Este modulo es un driver para BSP280");
MODULE_INFO(mse_imd, "Esto no es para simples mortales");

