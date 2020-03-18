package ble

import (
	"strings"
	"github.com/bettercap/gatt"
)

const (
	AnnolightID = "27041b71495d49629d6bec53fbc9c3c5"
	BatteryID   = "180F"
	BLEUartID   = "6e400001b5a3f393e0a9e50e24dcca9e"

	modeOff   = 'F'
	modeOn    = 'N'
	modeParty = 'P'

	wMask     uint64 = 0xFF
	rMask     uint64 = 0xFF << (8 * iota)
	gMask
	bMask
	modeMask
	sensRMask
	sensLMask
	sensTMask
)

var (
	BatteryUUID = gatt.MustParseUUID(BatteryID)
	BLEUartUUID = gatt.MustParseUUID(BLEUartID)
	//CoreTempUUID    = gatt.MustParseUUID("")
	//LightSensorUUID = gatt.MustParseUUID("")
	//LightStatusUUID = gatt.MustParseUUID("")
)

type AnnolightPeripheral struct {
	Battery     *gatt.Characteristic
	BLEUart     *gatt.Characteristic
	CoreTemp    *gatt.Characteristic
	LightSensor *gatt.Characteristic
	LightStatus *gatt.Characteristic
}

func OnStateChanged(d gatt.Device, s gatt.State) {
	switch s {
	case gatt.StatePoweredOn:
		logger.Info("Scanning...")
		d.Scan([]gatt.UUID{}, false)
		return
	default:
		d.StopScanning()
	}
}

type BLEHandler struct {
	d gatt.Device

	Annolight *AnnolightPeripheral
	Read      chan *ReadState
	Write     chan *WriteState
}

type ReadState struct {
	WBrightness uint8
	RBrightness uint8
	GBrightness uint8
	BBrightness uint8

	Mode        uint8
	LeftSensor  uint8
	RightSensor uint8
	CoreTemp    uint8
}

type WriteState struct {
	WBrightness, Mode uint8
}

func NewBLEHandler(d gatt.Device) *BLEHandler {
	bleHandler := &BLEHandler{d: d}
	bleHandler.d.Handle(
		gatt.PeripheralDiscovered(bleHandler.OnPeriphDiscovered),
		gatt.PeripheralConnected(bleHandler.OnPeriphConnected),
		gatt.PeripheralDisconnected(bleHandler.OnPeriphDisconnected),
	)
	return bleHandler
}

func (b *BLEHandler) OnPeriphDiscovered(p gatt.Peripheral, a *gatt.Advertisement, rssi int) {
	if strings.ToUpper(p.ID()) != AnnolightID {
		return
	}

	logger.Info("Found Annolight")
	p.Device().StopScanning()
	p.Device().Connect(p)
}

func (b *BLEHandler) OnPeriphConnected(p gatt.Peripheral, err error) {
	logger.Info("Annolight connected")
}

func (b *BLEHandler) OnPeriphDisconnected(p gatt.Peripheral, err error) {
	logger.Info("Annolight disconnected")
}
