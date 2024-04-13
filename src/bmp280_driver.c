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

/*
 * Definicion de los ID correspondientes al Device Tree. Estos deben ser informados al
 * kernel mediante la macro MODULE_DEVICE_TABLE
 *
 * NOTA: Esta seccion requiere que CONFIG_OF=y en el kernel
 */

static const struct of_device_id mse_dt_ids[] =
{
    { .compatible = "imd,mse_driver", },
    { /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, mse_dt_ids);


/* User is reading data from /dev/msedrvXX */
static ssize_t mse_read(struct file *file, char __user *userbuf, size_t count, loff_t *offset)
{
	struct mse_dev *mse;
	struct i2c_msg msg[2];
	static uint8_t in = 0xD0;



	msg[0].addr = DEVICE_ADDRESS;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &in;

	msg[1].addr = DEVICE_ADDRESS;
	msg[1].flags = I2C_M_RD | I2C_M_RECV_LEN;
	msg[1].len = 1;
	msg[1].buf = (unsigned char *)userbuf;

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    pr_info("mse_read() fue invocada.");

	i2c_transfer(mse->client->adapter, msg, 2);


	//pr_info("Read: in: %x, out: %x, len : %x", in, out, nbytes);

    return 0;
}

static ssize_t mse_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    struct mse_dev *mse;

	static uint8_t in;
	static uint8_t regcpy;

	static struct i2c_msg msg[] = {
		{ .addr = DEVICE_ADDRESS, .flags = 0, .len = 1, .buf = &regcpy },
		{ .addr = DEVICE_ADDRESS, .flags = 0, .len = 1, .buf = &in }
	};

    pr_info("mse_write() fue invocada.");

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    /*
     * Aqui ira las llamadas a i2c_transfer() que correspondan pasando
     * como dispositivo mse->client
	*/

	i2c_transfer(mse->client->adapter, msg, 2);


	//pr_info("Write: in: %x", in);

    return 0;
}

static long mse_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mse_dev *mse;

    pr_info("my_dev_ioctl() fue invocada. cmd = %d, arg = %ld\n", cmd, arg);

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    /*
     * Aqui ira las llamadas a i2c_transfer() que correspondan pasando
     * como dispositivo mse->client
    */


    return 0;
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

