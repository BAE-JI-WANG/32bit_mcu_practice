#include "sdslot.h"

#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"


#define SDC_SCK_PIN     29  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN    4  ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN    28  ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN      3//SPI_SS_PIN  ///< SDC chip select (CS) pin.


NRF_BLOCK_DEV_SDC_DEFINE(
        m_block_dev_sdc,
        NRF_BLOCK_DEV_SDC_CONFIG(
                SDC_SECTOR_SIZE,
                APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
         ),
         NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00")
);

static FATFS m_filesystem;

bool fatfs_init(void)
{
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    memset(&m_filesystem, 0, sizeof(FATFS));

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
        DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    printf("Initializing disk 0 (QSPI)...");
    //disk_state = disk_initialize(0);
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        printf("Disk initialization failed.");
        return false;
    }

    printf("Mounting volume...");
    ff_result = f_mount(&m_filesystem, "", 1);
    if (ff_result != FR_OK)
    {
        if (ff_result == FR_NO_FILESYSTEM)
        {
            printf("Mount failed. Filesystem not found. Please format device.");
        }
        else
        {
            printf("Mount failed: %u", ff_result);
        }
        return false;
    }

    return true;
}

void fatfs_uninit(void)
{
//	    NRF_LOG_INFO("Un-initializing disk 0 (QSPI)...");
    UNUSED_RETURN_VALUE(disk_uninitialize(0));
}

static bool fatfs_file_exist(char * fileName)
{
    DIR dir;
    FRESULT ff_result;
    FILINFO fno;

    ff_result = f_stat(fileName, &fno);

    if(ff_result == FR_OK)
    {
        return true;
    }
    else
    {
        return false;
    }
}


void fatfs_ls(void)
{
    DIR dir;
    FRESULT ff_result;
    FILINFO fno;

//	    if (m_usb_connected)
//	    {
//	        NRF_LOG_ERROR("Unable to operate on filesystem while USB is connected");
//	        return;
//	    }

    printf("\r\nListing directory: /");
    ff_result = f_opendir(&dir, "/");
    if (ff_result != FR_OK)
    {
        printf("Directory listing failed: %u", ff_result);
        return;
    }

    uint32_t entries_count = 0;
    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            printf("Directory read failed: %u", ff_result);
            return;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                printf("   <DIR>   %s\r\n",(uint32_t)fno.fname);
            }
            else
            {
                printf("%9lu  %s\r\n", fno.fsize, (uint32_t)fno.fname);
            }
        }

        ++entries_count;
        NRF_LOG_FLUSH();
    } while (fno.fname[0]);


    printf("Entries count: %u\r\n", entries_count);
}

static bool fatfs_check(void)
{
    DIR dir;
    FRESULT ff_result;
    FILINFO fno;

//	    if (m_usb_connected)
//	    {
//	        NRF_LOG_ERROR("Unable to operate on filesystem while USB is connected");
//	        return;
//	    }

    //printf("\r\nListing directory: /");
    ff_result = f_opendir(&dir, "/");
    if (ff_result != FR_OK)
    {
        printf("\r\n check fail \r\n");
        return false;
    }

    uint32_t entries_count = 0;
    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            printf("\r\n check fail 2 \r\n");
            return false;
        }

    } while (fno.fname[0]);

    
    //printf("Entries count: %u\r\n", entries_count);

    return true;
}


void fatfs_delete_all_file(void)
{
    DIR dir;
    FRESULT ff_result;
    FILINFO fno;

//	    if (m_usb_connected)
//	    {
//	        NRF_LOG_ERROR("Unable to operate on filesystem while USB is connected");
//	        return;
//	    }

    printf("\r\nListing directory: /");
    ff_result = f_opendir(&dir, "/");
    if (ff_result != FR_OK)
    {
        printf("Directory listing failed: %u", ff_result);
        return;
    }

    uint32_t entries_count = 0;
    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            printf("Directory read failed: %u", ff_result);
            return;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                printf("   <DIR>   %s\r\n",(uint32_t)fno.fname);
            }
            else
            {
                printf("%9lu  %s\r\n", fno.fsize, (uint32_t)fno.fname);
                FRESULT result = f_unlink(fno.fname);
                printf("\r\n delete : %d\r\n", result);
            }
        }

        ++entries_count;
        
        NRF_LOG_FLUSH();
    } while (fno.fname[0]);


    printf("Entries count: %u\r\n", entries_count);
}



#define FILE_NAME_2   "201105.txt"
#define TEST_STRING_2 "SD card example. TEST"


void fatfs_write(char * fileName, char * text, int bytes)
{

    static FIL file;
	
    FRESULT ff_result;
    uint32_t bytes_written;
    char text_val[bytes];
    strncpy(text_val, text, bytes);
    
    ff_result = f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        printf("Unable to open or create file: %c", fileName);
        return;
    }
    
    ff_result = f_write(&file, text_val, sizeof(text_val) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        printf("Write failed\r\n.");
    }
    else
    {
        printf("%d bytes written.", bytes_written);
    }
    
    (void) f_close(&file);
    return;

}

int fatfs_read(char * fileName, char * text, int bytes)
{
    static FIL file;
	
    FRESULT ff_result;
    uint32_t bytes_written;
    char text_val[bytes];
    strncpy(text_val, text, bytes);
    
    //ff_result = f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    ff_result = f_open(&file, fileName, FA_READ);
    if (ff_result != FR_OK)
    {
        printf("Unable to open or create file: %c", fileName);
        return 0;
    }
    
    ff_result = f_read(&file, text_val, sizeof(text_val) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        printf("Write failed\r\n.");
    }
    else
    {
        printf("\r\n %d bytes read.", bytes_written);
        if(bytes_written > 0) {
                //printf("\r\n read data : %s \r\n", text_val);
                printf("\r\n read data : %s\r\n", text_val);
        }
    }

    strncpy(text, text_val, bytes_written);
    
    (void) f_close(&file);
    return bytes_written;
	
}




static void fatfs_file_create(void)
{
    FRESULT ff_result;
    FIL file;
    char filename[16];

//	    if (m_usb_connected)
//	    {
//	        NRF_LOG_ERROR("Unable to operate on filesystem while USB is connected");
//	        return;
//	    }

    (void)snprintf(filename, sizeof(filename), "%08x.txt", rand());

    NRF_LOG_RAW_INFO("Creating random file: %s ...", (uint32_t)filename);
    NRF_LOG_FLUSH();

    ff_result = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (ff_result != FR_OK)
    {
        NRF_LOG_ERROR("\r\nUnable to open or create file: %u", ff_result);
        NRF_LOG_FLUSH();
        return;
    }

    ff_result = f_close(&file);
    if (ff_result != FR_OK)
    {
        NRF_LOG_ERROR("\r\nUnable to close file: %u", ff_result);
        NRF_LOG_FLUSH();
        return;
    }
    NRF_LOG_RAW_INFO("done\r\n");
}


