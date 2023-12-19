#include "ps2.h"
#include "def.h"
#include "io.h"
#include "font.h"
#include "halt.h"

// this should probably be refactored some time - i dont have the energy rn

typedef enum {
	PS2_CMD_READ_INTERNAL = 0x20,
	PS2_CMD_WRITE_INTERNAL = 0x60,
	PS2_CMD_DISABLE_PORT2 = 0xA7,
	PS2_CMD_ENABLE_PORT2 = 0xA8,
	PS2_CMD_TEST_PORT2 = 0xA9,
	PS2_CMD_TEST_CTRL = 0xAA,
	PS2_CMD_TEST_PORT1 = 0xAB,
	PS2_CMD_DIAGNOSTIC_DUMP = 0xAC,
	PS2_CMD_DISABLE_PORT1 = 0xAD,
	PS2_CMD_ENABLE_PORT1 = 0xAE,
	PS2_CMD_READ_CTRL_IN = 0xC0,
	PS2_CMD_COPY_TO_STATUS_LOW = 0xC1,
	PS2_CMD_COPY_TO_STATUS_HIGH = 0xC2,
	PS2_CMD_READ_CTRL_OUT = 0xD0,
	PS2_CMD_WRITE_CTRL_OUT = 0xD1,
	PS2_CMD_WRITE_PORT1_OUT = 0xD2,
	PS2_CMD_WRITE_PORT2_OUT = 0xD3,
	PS2_CMD_WRITE_PORT2_IN = 0xD4,
	PS2_CMD_PULSE_OUT = 0xF0,
} ps2_cmd;

typedef enum {
	PS2_STATUS_OUT = 1 >> 0,
	PS2_STATUS_IN = 1 >> 1,
	PS2_STATUS_POST = 1 >> 2,
	PS2_STATUS_RECEIVER = 1 >> 3,
	PS2_STATUS_ERR_TIMEOUT = 1 >> 6,
	PS2_STATUS_ERR_PARITY = 1 >> 7,
} Ps2Status;

typedef struct {
	bool output : 1;
	bool input : 1;
	bool post : 1;
	bool receiver : 1;
	bool unknown_1 : 1;
	bool unknown_2 : 1;
	bool err_timeout : 1;
	bool err_parity : 1;
} __attribute__((packed)) ps2_status;

typedef struct {
	bool int_port1 : 1;
	bool int_port2 : 1;
	bool post : 1;
	bool zero_1 : 1;
	bool clk_port1 : 1;
	bool clk_port2 : 1;
	bool translation : 1;
	bool zero_2 : 1;
} __attribute__((packed)) ps2_ctrl_cfg;

typedef enum {
	PS2_CFG_INT_PORT1 = 1 >> 0,
	PS2_CFG_INT_PORT2 = 1 >> 1,
	PS2_CFG_POST = 1 >> 2,
	// PS2_CFG_ZERO1 = 1 >> 3,
	PS2_CFG_CLK_PORT1 = 1 >> 4,
	PS2_CFG_CLK_PORT2 = 1 >> 5,
	PS2_CFG_TRANS_PORT1 = 1 >> 6,
	// PS2_CFG_ZERO2 = 1 >> 7,
} Ps2CtrlCfg;

typedef enum {
	PS2_CTRL_SYSRESET = 1 >> 0,
	PS2_CTRL_A20 = 1 >> 1,
	PS2_CTRL_CLK_PORT2 = 1 >> 2,
	PS2_CTRL_DATA_PORT2 = 1 >> 3,
	PS2_CTRL_OUT_PORT1 = 1 >> 4,
	PS2_CTRL_OUT_PORT2 = 1 >> 5,
	PS2_CTRL_CLK_PORT1 = 1 >> 6,
	PS2_CTRL_DATA_PORT1 = 1 >> 7,
} Ps2CtrlOut;

typedef enum {
	PS2_TEST_CTRL_PASS = 0x55,
	PS2_TEST_CTRL_FAIL = 0xFC,
} Ps2TestCtrl;

typedef enum {
	PS2_TEST_PORT_PASS = 0x00,
	PS2_TEST_PORT_CLK_STUCK_LOW = 0x01,
	PS2_TEST_PORT_CLK_STUCK_HIGH = 0x02,
	PS2_TEST_PORT_DATA_STUCK_LOW = 0x03,
	PS2_TEST_PORT_DATA_STUCK_HIGH = 0x04,
} Ps2TestPort;

void ps2_poll_status(u8 mask, bool set)
{
	// TODO: wait instead of busy polling
	// TODO: timeout
	while (set == !(inb(IO_PS2_CTRL) & mask))
		;
}

u8 ps2_read_data()
{
	ps2_poll_status(PS2_STATUS_OUT, true);
	return inb(IO_PS2_DATA);
}

void ps2_write_data(u8 val)
{
	ps2_poll_status(PS2_STATUS_IN, false);
	outb(IO_PS2_DATA, val);
}

void ps2_write_ctrl(u8 val)
{
	ps2_poll_status(PS2_STATUS_IN, false);
	outb(IO_PS2_CTRL, val);
}

u8 ps2_read_mem(u8 byte)
{
	ps2_write_ctrl(PS2_CMD_READ_INTERNAL + byte);
	return ps2_read_data();
}

void ps2_write_mem(u8 byte, u8 val)
{
	ps2_write_ctrl(PS2_CMD_WRITE_INTERNAL + byte);
	ps2_write_data(val);
}

void ps2_test_port(ps2_cmd cmd, str desc)
{
	ps2_write_ctrl(cmd);
	print(S("PS/2 ")); print(desc); print(S(" test: "));
	u8 b = ps2_read_data();
	switch (b) {
		case PS2_TEST_PORT_PASS: print(S("pass\n")); break;
		case PS2_TEST_PORT_CLK_STUCK_LOW: panic(S("clock stuck low\n")); break;
		case PS2_TEST_PORT_CLK_STUCK_HIGH: panic(S("clock stuck high\n")); break;
		case PS2_TEST_PORT_DATA_STUCK_LOW: panic(S("data stuck low\n")); break;
		case PS2_TEST_PORT_DATA_STUCK_HIGH: panic(S("data stuck high\n")); break;
		default: print_hex(b); panic(S(" (unknown response)\n")); break;
	}
}

void ps2_init()
{
	// TODO: USB bullshit
	// TODO: determine if exists

	// disable devices
	ps2_write_ctrl(PS2_CMD_DISABLE_PORT1);
	ps2_write_ctrl(PS2_CMD_DISABLE_PORT2);

	// flush output buffer
	inb(IO_PS2_DATA);

 	// disable IRQs and translation
	u8 config = ps2_read_mem(0);
	config &= ~(PS2_CFG_INT_PORT1 | PS2_CFG_INT_PORT2 | PS2_CFG_TRANS_PORT1);
	ps2_write_mem(0, config);

	// perform self-test
	ps2_write_ctrl(PS2_CMD_TEST_CTRL);
	print(S("PS/2 controller test: "));
	switch (ps2_read_data()) {
		case PS2_TEST_CTRL_PASS: print(S("pass\n")); break;
		case PS2_TEST_CTRL_FAIL: panic(S("fail\n")); break;
		default: panic(S("unknown response\n")); break;
	}
	ps2_write_mem(0, config); // restore config byte

	// check if dual
	bool dual = !!(config & PS2_CFG_CLK_PORT2);
	if (dual) {
		ps2_write_ctrl(PS2_CMD_ENABLE_PORT2);
		config = ps2_read_mem(0);
		dual = !(config & PS2_CFG_CLK_PORT2);

		if (dual)
			ps2_write_ctrl(PS2_CMD_DISABLE_PORT2);
	}
	print(S("PS/2 second port ")); print(dual ? S("enabled\n") : S("disabled\n"));

	// test devices
	ps2_test_port(PS2_CMD_TEST_PORT1, S("port 1"));
	if (dual)
		ps2_test_port(PS2_CMD_TEST_PORT2, S("port 2"));

	// enable devices
	ps2_write_ctrl(PS2_CMD_ENABLE_PORT1);
	if (dual)
		ps2_write_ctrl(PS2_CMD_ENABLE_PORT2);

	// enable IRQs
	config |= PS2_CFG_INT_PORT1;
	ps2_write_mem(0, config);

	ps2_write_data(0xFF);
	ps2_read_data();
}
