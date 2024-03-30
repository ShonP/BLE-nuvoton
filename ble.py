import bluetooth
import keyboard
import tkinter as tk
import threading
import time

# Global variable to store the received Bluetooth data
bluetooth_data = ""


class BluetoothGUI:
    def __init__(self):
        self.root = tk.Tk()
        self.devices_listbox = tk.Listbox(self.root, height=5, width=50)
        self.devices_listbox.pack()
        self.connect_button = tk.Button(
            self.root, text="Connect", command=self.connect_to_device
        )
        self.connect_button.pack()
        self.log_text = tk.Text(self.root, height=10, width=50)
        self.log_text.pack()
        self.sock = None

        # Discover nearby Bluetooth devices
        self.nearby_devices = bluetooth.discover_devices()

        # Display the available devices in the listbox
        for i, device in enumerate(self.nearby_devices):
            device_name = bluetooth.lookup_name(device)
            self.devices_listbox.insert(tk.END, f"{device_name} [{device}]")

    def connect_to_device(self):
        # Get the selected device from the listbox
        selected_index = self.devices_listbox.curselection()[0]
        selected_device = self.nearby_devices[selected_index]

        # Connect to the selected device
        port = 1  # RFCOMM port
        self.sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        self.sock.connect((selected_device, port))

        self.append_log("Connected. Type 'exit' to disconnect.")

        # Start the Bluetooth communication loop in a separate thread
        threading.Thread(target=self.bluetooth_thread).start()

        # Start the keyboard control loop in a separate thread
        threading.Thread(target=self.keyboard_thread).start()

    def bluetooth_thread(self):
        global bluetooth_data

        if self.sock is None:
            self.append_log(
                "No device selected. Please select a device and click 'Connect'."
            )
            return

        # Send and receive messages
        while True:
            received_data = self.sock.recv(1024)
            decoded_data = received_data.decode()

            print(f"DEBUG {decoded_data}")
            self.append_log(f"DEBUG {decoded_data}")

            # Save the decoded_data in the global variable
            bluetooth_data = decoded_data

            if decoded_data.lower() == "exit":
                break

        # Disconnect
        self.sock.close()
        print("Disconnected.")
        self.append_log("Disconnected.")

    def keyboard_thread(self):
        global bluetooth_data

        while True:
            # Check if there is any Bluetooth data available
            if bluetooth_data:
                # Handle direction change
                while bluetooth_data.startswith("UP"):
                    keyboard.press_and_release("up")
                    time.sleep(0.5)
                while bluetooth_data.startswith("DOWN"):
                    keyboard.press_and_release("down")
                    time.sleep(0.5)
                while bluetooth_data.startswith("LEFT"):
                    keyboard.press_and_release("left")
                    time.sleep(0.5)
                while bluetooth_data.startswith("RIGHT"):
                    keyboard.press_and_release("right")
                    time.sleep(0.5)
                if bluetooth_data.startswith("SPACE"):
                    keyboard.press_and_release("space")
                # Reset the Bluetooth data after processing
                bluetooth_data = ""

    def append_log(self, text):
        self.log_text.insert(tk.END, text + "\n")
        self.log_text.see(tk.END)
        print(text)  # Log to the terminal


if __name__ == "__main__":
    gui = BluetoothGUI()
    gui.root.mainloop()
