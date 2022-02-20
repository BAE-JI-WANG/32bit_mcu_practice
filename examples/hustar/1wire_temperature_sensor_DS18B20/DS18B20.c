#include "DS18B20.h"

/** Structure holding a 1-wire serial number. */
typedef struct sBSPACMonewireSerialNumber
{
	/** The serial number in order MSB to LSB */
	uint8_t id[6];
} sBSPACMonewireSerialNumber;

enum
{
	BSPACM_ONEWIRE_CMD_READ_ROM = 0x33,						/** Read 64-bit ROM code without using search procedure */
	BSPACM_ONEWIRE_CMD_SKIP_ROM = 0xcc,						/** Skip ROM sends the following command to all bus devices */
	BSPACM_ONEWIRE_CMD_READ_POWER_SUPPLY = 0xb4,			// 確定設備是寄生供電還是外部供電
	BSPACM_ONEWIRE_CMD_RECALL_EE = 0xb8,					/** Store data from EEPROM into RAM */
	BSPACM_ONEWIRE_CMD_READ_SCRATCHPAD = 0xbe,				/** Read the RAM area. */

	BSPACM_ONEWIRE_CMD_CONVERT_T = 0x44,					// Initiate a temperature conversion.
															// 溫度轉換可能需要長達750毫秒, 默認12位解析度
															// 對於外部（非寄生蟲）供電的傳感器，呼叫者可以使用 #iBSPACMonewireReadBit_ni
															// 確定是否轉換已經完成。 設備響應時指示完成帶1. * /
};

/** Define protocol state times in microseconds.
 *
 * @note Since all these times are far less than any sane watchdog
 * interval, and the timing can be important, BSPACM_CORE_DELAY_CYCLES
 * is not used in this module. */
enum
{
	OWT_RSTL_us = 480,						/** Minimum time to hold bus low to ensure reset */
	OWT_RSTH_us = 480,						/** Time to wait for presence detection after reset to quiesce */
	OWT_PDHIGH_us = 60,						/** Delay before presence pulse is known to be valid (15us..60us) */
	OWT_LOW0_us = 60,						/** Minimum time to hold bus low when writing a zero */
	OWT_LOW1_us = 1,						/** Minimum time to hold bus low when writing a one */
	OWT_REC_us = 1,							/** Recovery delay between write/read transaction cycles */
	OWT_INT_us = 1,							/** Time to hold bus low when initiating a read slot */
	OWT_RDV_us = 15 - OWT_INT_us,			/** Point at which read value should be sampled */
	OWT_SLOT_us = 60,						/** Minimum duration of a read or write slot */
};



uint64_t onewire_pin_number = ONEWIRE_PIN;
#define ONEWIRE_BIT (1ULL << ONEWIRE_PIN)
//	uint64_t ONEWIRE_BIT = (1ULL << onewire_pin_number);

//	#ifndef ONEWIRE_PIN
//	#define ONEWIRE_PIN 24//26
//	#endif /* ONEWIRE_PIN */



//#define ONEWIRE_BIT (1ULL << onewire_pin_number)

void dirset (void)
{
	NRF_GPIO->DIRSET = ONEWIRE_BIT;
}

void dirclr (void)
{
	NRF_GPIO->PIN_CNF[onewire_pin_number] = 0
		| (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
		| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
		| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
		| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
		| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
}

int iBSPACMonewireReset_ni (void)
{
	int present;

#if 1
	/* Non-standard: Hold bus high for OWT_RESET_us.	This provides
	 * enough parasitic power for the device to signal presence.
	 * Without this, effective RSTL duration may exceed the maximum
	 * 960us before device reset. */
	dirset();
	NRF_GPIO->OUTSET = ONEWIRE_BIT;
	nrf_delay_us(OWT_RSTH_us);

	/* Hold bus low for OWT_RESET_us */
	NRF_GPIO->OUTCLR = ONEWIRE_BIT;
	nrf_delay_us(OWT_RSTL_us);

	/* Release bus and switch to input until presence pulse should be
	 * visible. */
	dirclr();
	nrf_delay_us(OWT_PDHIGH_us);

	/* Record presence if bus is low (DS182x is holding it there) */
	present = !(NRF_GPIO->IN & ONEWIRE_BIT);

	/* Wait for reset cycle to complete */
	nrf_delay_us(OWT_RSTH_us - OWT_PDHIGH_us);
#else
	/* Non-standard: Hold bus high for OWT_RESET_us.	This provides
	 * enough parasitic power for the device to signal presence.
	 * Without this, effective RSTL duration may exceed the maximum
	 * 960us before device reset. */
	nrf_gpio_cfg_output(ONEWIRE_PIN);
	nrf_gpio_pin_set(ONEWIRE_PIN);
	nrf_delay_us(OWT_RSTH_us);

	/* Hold bus low for OWT_RESET_us */
	nrf_gpio_pin_clear(ONEWIRE_PIN);
	nrf_delay_us(OWT_RSTL_us);

	nrf_gpio_cfg_input(ONEWIRE_PIN, NRF_GPIO_PIN_NOPULL);			// 釋放總線並切換到輸入，直到存在脈衝應該可見
	nrf_delay_us(OWT_PDHIGH_us);

	/* Record presence if bus is low (DS182x is holding it there) */
	present = !(nrf_gpio_pin_read(ONEWIRE_PIN));
#endif
	/* Wait for reset cycle to complete */
	nrf_delay_us(OWT_RSTH_us - OWT_PDHIGH_us);

	return present;
}

void vBSPACMonewireShutdown_ni (void)
{
	NRF_GPIO->OUTCLR = ONEWIRE_BIT;
	dirclr();
}

void vBSPACMonewireWriteByte_ni (int byte)
{
	int bp;

	for (bp = 0; bp < 8; ++bp)
	{
		NRF_GPIO->OUTCLR = ONEWIRE_BIT;
		dirset();
		if (byte & 0x01) {
			nrf_delay_us(OWT_LOW1_us);
			dirclr();
			nrf_delay_us(OWT_SLOT_us - OWT_LOW1_us + OWT_REC_us);
		} else {
			nrf_delay_us(OWT_LOW0_us);
			dirclr();
			nrf_delay_us(OWT_SLOT_us - OWT_LOW0_us + OWT_REC_us);
		}
		byte >>= 1;
	}
}

int iBSPACMonewireReadBit_ni (void)
{
	int rv;

	NRF_GPIO->OUTCLR = ONEWIRE_BIT;
	dirset();
	nrf_delay_us(OWT_INT_us);
	dirclr();
	nrf_delay_us(OWT_RDV_us);
	//abc	vBSPACMledSet(0, 1);
	rv = !!(NRF_GPIO->IN & ONEWIRE_BIT);
	//abc	vBSPACMledSet(0, 0);
	nrf_delay_us(OWT_SLOT_us - OWT_RDV_us - OWT_INT_us + OWT_REC_us);
	return rv;
}

int iBSPACMonewireReadByte_ni (void)
{
	int byte = 0;
	int bit = 1;

	do {
		if (iBSPACMonewireReadBit_ni()) {
			byte |= bit;
		}
		bit <<= 1;
	} while (0x100 != bit);
	return byte;
}

int iBSPACMonewireComputeCRC(const unsigned char * romp, int len)
{
	static const unsigned char OW_CRC_POLY = 0x8c;
	unsigned char crc = 0;

	while (0 < len--) {
		int bi;

		crc ^= *romp++;
		for (bi = 0; bi < 8; ++bi) {
			if (crc & 1) {
				crc = (crc >> 1) ^ OW_CRC_POLY;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
}

int iBSPACMonewireReadSerialNumber (sBSPACMonewireSerialNumber * snp)
{
	uint8_t rom[8];
	int i;
	int rv = -1;

	do {
		if (! iBSPACMonewireReset_ni()) {
			break;
		}
		vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_READ_ROM);
		for (i = 0; i < sizeof(rom); ++i) {
			rom[i] = iBSPACMonewireReadByte_ni();
		}
		rv = 0;
	} while (0);

	if (0 == rv) {
		if (0 != iBSPACMonewireComputeCRC(rom, sizeof(rom))) {
			rv = -1;
		} else {
			for (i = 0; i < sizeof(snp->id); ++i) {
				snp->id[i] = rom[sizeof(rom) - 2 - i];
			}
		}
	}
	return rv;
}

int iBSPACMonewireRequestTemperature_ni (void)
{
	if (! iBSPACMonewireReset_ni()) {
		return -1;
	}
	vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_SKIP_ROM);
	vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_CONVERT_T);
	return 0;
}

int iBSPACMonewireReadPowerSupply (void)
{
	int rv = -1;

	if (iBSPACMonewireReset_ni())
	{
		vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_SKIP_ROM);
		vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_READ_POWER_SUPPLY);
		rv = iBSPACMonewireReadBit_ni();
	}
	return rv;
}

int iBSPACMonewireReadTemperature_ni (int * temp_xCel)
{
	int t;

	if (! iBSPACMonewireReset_ni()) {
		return -1;
	}
	vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_SKIP_ROM);
	vBSPACMonewireWriteByte_ni(BSPACM_ONEWIRE_CMD_READ_SCRATCHPAD);
	t = iBSPACMonewireReadByte_ni();
	t |= (iBSPACMonewireReadByte_ni() << 8);
	*temp_xCel = t;
	return 0;
}

void DS18B20_Run(void)
{
	do
	{
		if (! iBSPACMonewireReset_ni()) {
			printf("ERR: No DS18B20 present on P0.%u\n", onewire_pin_number);
			break;
		}

		static const char * const supply_type[] = { "parasitic", "external" };

		int external_power = iBSPACMonewireReadPowerSupply();
		printf("Power supply: %s\r\n", supply_type[external_power]);
		if (0 > external_power) {
			printf("ERROR: Device not present?\r\n");
			break;
		}

		sBSPACMonewireSerialNumber serial;
		int rc = iBSPACMonewireReadSerialNumber(&serial);
		printf("Serial got %d: \r\n", rc);
		//abc	vBSPACMconsoleDisplayOctets(serial.id, sizeof(serial.id));
		//abc	putchar('\n');
#if 1
		while (0 == iBSPACMonewireRequestTemperature_ni())
		{
			int t_raw;
			nrf_delay_ms(600UL);
			while (! iBSPACMonewireReadBit_ni()) {
			}
			rc = iBSPACMonewireReadTemperature_ni(&t_raw);
			int t_dCel = (10 * t_raw) / 16;
			int t_dFahr = 320 + (9 * t_dCel) / 5;
			printf("Got %d dCel, %d d[Fahr]\r\n", t_dCel, t_dFahr);
		}
#endif
	} while (0);
	dirset();
	NRF_GPIO->OUTCLR = ONEWIRE_BIT;

}

float DS18B20_get_temperature()
{
    if (! iBSPACMonewireReset_ni()) {
		printf("ERR: No DS18B20 present on P0.%u\n", onewire_pin_number);
		return 0;
	}

	static const char * const supply_type[] = { "parasitic", "external" };

	int external_power = iBSPACMonewireReadPowerSupply();
	//printf("Power supply: %s\r\n", supply_type[external_power]);
	if (0 > external_power) {
		printf("ERROR: Device not present?\r\n");
		return 0;
	}

	sBSPACMonewireSerialNumber serial;
	int rc = iBSPACMonewireReadSerialNumber(&serial);
	//printf("Serial got %d: \r\n", rc);
	//abc	vBSPACMconsoleDisplayOctets(serial.id, sizeof(serial.id));
	//abc	putchar('\n');
#if 1
    int t_dCel = 0;
	if (0 == iBSPACMonewireRequestTemperature_ni())
	{
		int t_raw;
		nrf_delay_ms(600UL);
		while (! iBSPACMonewireReadBit_ni()) {
		}
		rc = iBSPACMonewireReadTemperature_ni(&t_raw);
		t_dCel = (10 * t_raw) / 16;
		int t_dFahr = 320 + (9 * t_dCel) / 5;
		//printf("Got %d dCel, %d d[Fahr]\r\n", t_dCel, t_dFahr);
	}
#endif

    return ((float)t_dCel/10);
}

void DS18B20_request_temperature()
{
    if (! iBSPACMonewireReset_ni()) {
		printf("ERR: No DS18B20 present on P0.%u\n", onewire_pin_number);
		return;
	}

	static const char * const supply_type[] = { "parasitic", "external" };

	int external_power = iBSPACMonewireReadPowerSupply();
	//printf("Power supply: %s\r\n", supply_type[external_power]);
	if (0 > external_power) {
		printf("ERROR: Device not present?\r\n");
		return;
	}

	sBSPACMonewireSerialNumber serial;
	int rc = iBSPACMonewireReadSerialNumber(&serial);
	//printf("Serial got %d: \r\n", rc);
	//abc	vBSPACMconsoleDisplayOctets(serial.id, sizeof(serial.id));
	//abc	putchar('\n');
#if 1
    int t_dCel = 0;
	if (0 == iBSPACMonewireRequestTemperature_ni())
	{
//			int t_raw;
//			nrf_delay_ms(600UL);
//			while (! iBSPACMonewireReadBit_ni()) {
//			}
//			rc = iBSPACMonewireReadTemperature_ni(&t_raw);
//			t_dCel = (10 * t_raw) / 16;
//			int t_dFahr = 320 + (9 * t_dCel) / 5;
		//printf("Got %d dCel, %d d[Fahr]\r\n", t_dCel, t_dFahr);
	}
#endif
}

float DS18B20_read_temperature()
{
    int t_dCel = 0;
    int t_raw;

    while (! iBSPACMonewireReadBit_ni()) {
	}
	int rc = iBSPACMonewireReadTemperature_ni(&t_raw);
	t_dCel = (10 * t_raw) / 16;
	int t_dFahr = 320 + (9 * t_dCel) / 5;
    
    return ((float)t_dCel/10);
}
