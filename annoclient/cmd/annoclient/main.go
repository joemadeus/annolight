package main

import (
	"flag"
	"github.com/bettercap/gatt"
	"github.com/joemadeus/annoclient/ble"
)

var (
	onOffFlag      = flag.String("o", "on", "on/off")
	brightnessFlag = flag.Int64("b", -1, "0..255")
	partyModeFlag  = flag.String("p", "off", "on/off")

	BLEOptions = []gatt.Option{gatt.MacDeviceRole(gatt.CentralManager)}
)

func main() {
	flag.Parse()

	d, err := gatt.NewDevice(BLEOptions...)
	if err != nil {
		logger.Fatal("Failed to open device, err: %s\n", err)
		return
	}

	bleHandler := ble.NewBLEHandler(d)
	d.Init(ble.OnStateChanged)

	bleHandler.Write
}
