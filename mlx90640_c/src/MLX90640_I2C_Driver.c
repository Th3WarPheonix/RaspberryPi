#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "i2c.h"
#include "MLX90640_I2C_Driver.h"

#define MLX_I2C_ADDR 0x33

static struct i2c_device g_handle_obj;
static struct i2c_device *g_handle;

// int MLX90640_I2CInit() {
//     char *filename = "/dev/i2c-1";

//     i2c_fd = open(filename, O_RDWR);
//     if (i2c_fd < 0) {
//         perror("Failed to open I2C device");
//         return -1;
//     }
//     if (ioctl(i2c_fd, I2C_SLAVE, MLX_I2C_ADDR) < 0) {
//         perror("Failed to set I2C slave address");
//         close(i2c_fd);
//         return -1;
//     }
//     printf("MLX90640 initialized on %s, slave 0x%X\n", filename, MLX_I2C_ADDR);
//     return 0;
// }

struct MLX90640DriverRegister_t *
MLX90640_get_register(void)
{
  // This function returns a pointer to a static struct MLX90640DriverRegister_t,
  // which acts as a driver interface containing function pointers to all I2C operations.
  // It allows modular access to the MLX90640 I2C driver, enabling other code to call
  // driver functions indirectly (e.g., for abstraction or swapping implementations).
  static struct MLX90640DriverRegister_t reg;

  strcpy(reg.name_, "/dev/i2c-");
  reg.MLX90640_get_i2c_handle_  = MLX90640_get_i2c_handle;
  reg.MLX90640_set_i2c_handle_  = MLX90640_set_i2c_handle;
  reg.MLX90640_I2CInit_         = MLX90640_I2CInit;
  reg.MLX90640_I2CClose_        = MLX90640_I2CClose;
  reg.MLX90640_I2CRead_         = MLX90640_I2CRead;
  reg.MLX90640_I2CFreqSet_      = MLX90640_I2CFreqSet;
  reg.MLX90640_I2CGeneralReset_ = MLX90640_I2CGeneralReset;
  reg.MLX90640_I2CWrite_        = MLX90640_I2CWrite;
  return &reg;
}

int MLX90640_I2CInit() 
{
    if (g_handle == NULL)
    {
        g_handle = &g_handle_obj;
    }
    int bus;

    char *i2cPort = "/dev/i2c-1";
    
    printf ("I2C device tree: '%s'\n", i2cPort);
    if ((bus = i2c_open(i2cPort))==-1){
        perror("Failed to open I2C device");
        return -1;
    }
    else{
        memset(g_handle, 0, sizeof(struct i2c_device));

        g_handle->bus = bus;       // Bus 1
        g_handle->addr = 0x33;     // Slave address is 0x33, 7bit
        g_handle->iaddr_bytes = 2;  // Device internal addres = 2 bytes
        g_handle->page_bytes = 16;  // Device are capable of 16 bytes per page 
    }
    printf("MLX90640 initialized on %s, slave 0x%X\n", i2cPort, MLX_I2C_ADDR);
    return 0;
}

void *
MLX90640_get_i2c_handle(void)
{
  return (void *)g_handle;
}

void
MLX90640_set_i2c_handle(void *handle)
{
  g_handle = (struct i2c_device *)handle;
}

void MLX90640_I2CClose(void)
{
    int bus;
    bus = g_handle->bus;
    i2c_close(bus);
}

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddr, uint16_t nMemAddressRead, uint16_t *data)
{
    unsigned char buffer[1664];
    uint16_t *p;
    int i=0;

    memset(buffer, 0, sizeof(buffer));
    g_handle->addr = slaveAddr;
    
    p=data;

    i2c_ioctl_read(g_handle, startAddr, buffer, 2*nMemAddressRead);
    
    for (int cnt=0; cnt<nMemAddressRead; cnt++){
        i = cnt <<1;
        *p++=(uint16_t)buffer[i]*256 +(uint16_t)buffer[i+1];
    }
    return 0;
}

void MLX90640_I2CFreqSet(int freq)
{
  if (freq) {}
}

int MLX90640_I2CGeneralReset(void)
{
    return 0;
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    unsigned char buffer[4];
    memset(buffer, 0, sizeof(buffer));
    
    buffer[0]=(char)((data>>8)&0x00FF);
    buffer[1]=(char)(data&0x00FF);

    g_handle->addr = slaveAddr;

    i2c_ioctl_write(g_handle, writeAddress, buffer, 4);

    return 0;
}