In order for the I2C and I2S to work together, you need to follow these steps:

1. 
Find the i2c.c source file.
This file can be found in your esp-idf library.
Then under the folder "components", there is a folder called "driver".
Open this folder and open the i2c.c file.

2.
Once you opened the file go to the end of this function: i2c_driver_install
You can find that by using CTRL + F, and then searching for "ESP_LOGE(I2C_TAG, I2C_DRIVER_ERR_STR);"

3.
Add the following code above this statement but under the "else {":

    if (mode == I2C_MODE_MASTER) {
        ESP_LOGW(I2C_TAG, "BUGFIX: I2C driver already installed");
        return ESP_OK;
    }

4. 
When you did this it should look something like this:

    } else {
        if (mode == I2C_MODE_MASTER) {
            ESP_LOGW(I2C_TAG, "BUGFIX: I2C driver already installed");
            return ESP_OK;
        }
        ESP_LOGE(I2C_TAG, I2C_DRIVER_ERR_STR);
        return ESP_FAIL;
    }

5.
Save the file and you should be good to go.
