from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.button import Button
from kivy.uix.label import Label
from able import BluetoothDispatcher, GATT_SUCCESS
from jnius import autoclass

BluetoothGatt = autoclass('android.bluetooth.BluetoothGatt')

SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
CHARACTERISTIC_UUID = "87654321-4321-6789-4321-fedcba987654"

class BLEApp(App, BluetoothDispatcher):
    connected_characteristic = None

    def build(self):
        self.label = Label(text="Press to scan and connect")
        layout = BoxLayout(orientation='vertical')
        layout.add_widget(self.label)

        scan_button = Button(text="Scan and Connect")
        scan_button.bind(on_press=self.scan_and_connect)
        layout.add_widget(scan_button)

        disconnect_button = Button(text="Dissconnect")
        disconnect_button.bind(on_press=self.disconnect)
        layout.add_widget(disconnect_button)

        write_on_button = Button(text="Write LED ON")
        write_on_button.bind(on_press=self.write_on_button)
        layout.add_widget(write_on_button)

        write_off_button = Button(text="Write LED OFF")
        write_off_button.bind(on_press=self.write_off_button)
        layout.add_widget(write_off_button)

        return layout

    def scan_and_connect(self, instance):
        self.label.text = "Scanning..."
        self.start_scan()

    def disconnect(self, instance):
        if self.connected_characteristic is None:
            return
        self.label.text = "Disconnected..."
        self.close_gatt()
        self.connected_characteristic = None

    def on_device(self, device, rssi, advertisement):
        print(f"Discovered device: {device}, RSSI: {rssi}")
        # Replace "TargetDeviceName" with the actual name of your device
        device_address = device.getAddress()
        print(f"Discovered device Address: {device_address}")
        if device_address == "D4:D4:DA:96:6C:DE":
            self.stop_scan()
            self.connect_gatt(device)

    def on_connection_state_change(self, status, state):
        print(f"on_connection_state_change device status: {status}")
        if state == BluetoothGatt.STATE_CONNECTED:
            self.label.text = "Connected"
            self.discover_services()
        elif state == BluetoothGatt.STATE_DISCONNECTED:
            self.label.text = "Disconnected"

    def on_services(self, status, services):
        print(f"on_services device status: {status}")
        if status == BluetoothGatt.GATT_SUCCESS:
            self.label.text = "Services discovered"
            print(f"on_services services uuid: {services}")

            characteristic = services.search(CHARACTERISTIC_UUID)
            print(f"on_services device uuid: {characteristic}")
            if characteristic:
                self.label.text = "Characteristic found"
                self.connected_characteristic = characteristic

    def write_on_button(self, instance): 
        if self.connected_characteristic is None:
            return
        self.write_characteristic(self.connected_characteristic, b"0")

    def write_off_button(self, instance): 
        if self.connected_characteristic is None:
            return
        self.write_characteristic(self.connected_characteristic, b"1")


    def on_characteristic_write(self, characteristic, status):
        if status == GATT_SUCCESS:
            self.label.text = "Write successfull"

if __name__ == '__main__':
    BLEApp().run()