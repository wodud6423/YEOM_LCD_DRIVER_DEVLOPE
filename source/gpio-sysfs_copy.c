#include <linux/module.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/device.h>

#include <linux/kernel.h>
#include <linux/string.h>

#include <linux/platform_device.h>

#include <linux/uaccess.h>
#include <linux/io.h>

#include <linux/of.h>
#include <linux/of_device.h>

#include <linux/gpio/consumer.h>

#include "gpio_copy.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : "fmt,__func__

struct gdev_private_data {
    char label[32];
    struct gpio_desc * desc;
};

struct gdrv_private_data {
    int total_devices;
    struct class* class_gpio;
    struct device** device_gpio;
};

/* Driver Private Data */
struct gdrv_private_data gpio_drv_data;

struct of_device_id gpio_pdrv_match[] = {
    {.compatible = "org,bone-gpio-sysfs"},
    { }
};

ssize_t direction_show(struct device* dev,struct device_attribute* attr,char* buf)
{
    struct gdev_private_data* gpdev_data= (struct gdev_private_data*)dev_get_drvdata(dev);

    int dir = gpiod_get_direction(gpdev_data->desc); 
    if(dir < 0)
        return dir;
    char * direction = (dir == 0) ? "out" : "in";

    return sprintf(buf,"%s\n",direction);

}

ssize_t direction_store(struct device* dev, struct device_attribute* attr,const char* buf, size_t count)
{
    struct gdev_private_data* gpdev_data = (struct gdev_private_data*)dev_get_drvdata(dev);
    
    int ret = count;
    int result;

    if(sysfs_streq(buf,"out"))
    {
        result = gpiod_direction_output(gpdev_data->desc,0);
        if(result)
        {
            dev_err(dev,"Failed to Config direction output! \n");
        }
        return ret;
    }
    if(sysfs_streq(buf,"in"))
    {
        result = gpiod_direction_input(gpdev_data->desc);
        if(result)
        {
            dev_err(dev,"Failed to config direction input \n");
        }
        return ret;
    }
    dev_err(dev,"Failed to config direction! Check input direction mode (out or in) \n");
    return ret ? : count;
}

ssize_t value_show(struct device* dev, struct device_attribute* attr,char * buf)
{
    struct gdev_private_data * gpdev_data = (struct gdev_private_data*)dev_get_drvdata(dev);

    int val = gpiod_get_value(gpdev_data->desc);
    
    return sprintf(buf,"%d\n",val);
}
ssize_t value_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
    struct gdev_private_data * gpdev_data = (struct gdev_private_data *)dev_get_drvdata(dev);

    int value;
    if(kstrtoint(buf,10,&value) )
    {
	    dev_info(dev,"Failed to allocate value in kernel buffer! \n");
    }
    gpiod_set_value(gpdev_data->desc,value);
    
    return count;
}

ssize_t label_show(struct device* dev, struct device_attribute* attr,char* buf)
{
    struct gdev_private_data * gpdev_data = (struct gdev_private_data* )dev_get_drvdata(dev);
    
    char * label = (gpdev_data->label);
    
    if(!label)
    {
        dev_err(dev,"Failed to get gpio label! \n");
    }

    return sprintf(buf,"%s\n",label);
}

DEVICE_ATTR_RW(direction);
DEVICE_ATTR_RW(value);
DEVICE_ATTR_RO(label);

struct attribute *attrs_group[] = {
    &dev_attr_direction.attr,
    &dev_attr_value.attr,
    &dev_attr_label.attr,
    NULL
};
struct attribute_group gpio_attr_group = {
    .attrs = attrs_group
};
const struct attribute_group * gpio_attr_groups[] = {
    &gpio_attr_group,
    NULL
};

int gpio_pdrv_remove(struct platform_device * pdev)
{
    int i;
    
    dev_info(&pdev->dev,"Module Remove called! \n");
    
    for(i = 0 ; i < gpio_drv_data.total_devices; i++)
    {
        device_unregister(gpio_drv_data.device_gpio[i]);
    }

    return 0;
}

int gpio_pdrv_probe(struct platform_device * pdev)
{   
    int ret,i=0;

    const char *name;
    struct device * dev = &pdev->dev;

    struct device_node* parent = dev->of_node;
    struct device_node* child = NULL;
    struct gdev_private_data* gdev_data;
    
    /* First, total device update need! */
    gpio_drv_data.total_devices = of_get_child_count(parent);

    if(!gpio_drv_data.total_devices)
    {
        dev_err(dev,"No Match Child Device ! \n");
        ret = -EINVAL;
        return ret;
    }
    
    gpio_drv_data.device_gpio = devm_kzalloc(dev,sizeof(struct device*) * gpio_drv_data.total_devices,GFP_KERNEL);

    if(!gpio_drv_data.device_gpio){
        dev_err(dev,"Failed to allocate memory for GPIO Device! \n");
        ret = -ENOMEM;
        return ret;
    }

    for_each_available_child_of_node(parent,child)
    {
        gdev_data = devm_kzalloc(dev,sizeof(*gdev_data),GFP_KERNEL);

        if(!gdev_data)
        {
            dev_err(dev,"Failed to allocate memory for gpio private data struct ! \n");
            ret = -EINVAL;
	    return ret;
        }
        if(of_property_read_string(child,"label",&name) == 0 && name)
        {
		dev_info(dev,"GPIO label : %s",name);
		// strscpy(gdev_data->label,name,sizeof(gdev_data->label));
		snprintf(gdev_data->label,sizeof(gdev_data->label),"%s",name);

        }else{
		dev_warn(dev,"Not found property name  ! \n");
		snprintf(gdev_data->label,sizeof(gdev_data->label),"unkngpio%d",i);
	} 
        
        gdev_data->desc = devm_fwnode_get_gpiod_from_child(dev,"bone",&child->fwnode,GPIOD_ASIS,gdev_data->label);
        /* Latest Linux Version Not Support devm_fwnode_get_gpiod_from_child! */
	// gdev_data->desc = devm_fwnode_gpiod_get_index(dev,child,"bone",0,GPIOD_ASIS,gdev_data->label);

        if(IS_ERR(gdev_data->desc))
        {
            dev_err(dev,"Failed to receive gpio description! \n");
            ret = PTR_ERR(gdev_data->desc);
            if(ret == -ENOENT)
            {
                dev_err(dev,"No GPIO has been assigned to the requested function and/or index \n");
            }
            return ret;
        }

        /* Config Default gpio pin Direction */
        ret = gpiod_direction_output(gdev_data->desc,0);
        if(ret)
        {
            dev_err(dev,"Failed to config direction gpio , Check DT ! \n");
            return ret;
        }
        
        /* Config platform driver private data and create device in sysfs ! But Not create in /dev */
        gpio_drv_data.device_gpio[i] = device_create_with_groups(gpio_drv_data.class_gpio,dev,0,gdev_data,gpio_attr_groups,gdev_data->label);

        if(IS_ERR(gpio_drv_data.device_gpio[i]))
        {
            dev_err(dev,"Failed to create device groups! \n");
            ret = PTR_ERR(gpio_drv_data.device_gpio[i]);
            return ret;
        }
        i++;

    }

    return 0;
}


struct platform_driver gpio_pdrv = {
    .probe = gpio_pdrv_probe,
    .remove = gpio_pdrv_remove,
    .driver = {
        .name = "bone-gpios",
        .of_match_table = of_match_ptr(gpio_pdrv_match)
    }
};

int __init gpio_pdrv_init(void)
{
    /* First class create (We dont need to create device Number ! )*/
    int ret;
    gpio_drv_data.class_gpio = class_create(THIS_MODULE,"bone-gpios");
    
    if(IS_ERR(gpio_drv_data.class_gpio))
    {
        pr_info("Failed to create class /sys/bone-gpios! \n");
        ret = PTR_ERR(gpio_drv_data.class_gpio);
        return ret;
    }

    /* Second Register platform_driver_register*/
    platform_driver_register(&gpio_pdrv);
    pr_info("Successful Module init! \n");

    return 0;
}

void __exit gpio_pdrv_exit(void)
{
    platform_driver_unregister(&gpio_pdrv);
    
    class_destroy(gpio_drv_data.class_gpio);
    
    pr_info("Successful Module Exit ! \n");
    
    return;

}

module_init(gpio_pdrv_init);
module_exit(gpio_pdrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wodud043@naver.com");
MODULE_DESCRIPTION("This is Module for testing!");
