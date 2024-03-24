import bluetooth
import keyboard
import tkinter as tk
import threading

class BluetoothGUI:
    def __init__(self):
        self.root = tk.Tk()
        self.devices_listbox = tk.Listbox(self.root, height=5, width=50)
        self.devices_listbox.pack()
        self.connect_button = tk.Button(self.root, text="Connect", command=self.connect_to_device)
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
        threading.Thread(target=bluetooth_terminal, args=(self,)).start()

    def append_log(self, text):
        self.log_text.insert(tk.END, text + "\n")
        self.log_text.see(tk.END)
        print(text)  # Log to the terminal

def bluetooth_terminal(gui):
    if gui.sock is None:
        gui.append_log("No device selected. Please select a device and click 'Connect'.")
        return

    # Initialize the last direction as None
    last_direction = None

    # Send and receive messages
    while True:
        received_data = gui.sock.recv(1024)
        decoded_data = received_data.decode()
        
        print(f"DEBUG {decoded_data}")
        gui.append_log(f"DEBUG {decoded_data}")

        if decoded_data.startswith("STATIC"):
            gui.append_log(f"<< {decoded_data}")
            # Reset last_direction to None when "STATIC" is received
            last_direction = None
        elif decoded_data.startswith("UP") and last_direction != "up":
            last_direction = "up"
        elif decoded_data.startswith("DOWN") and last_direction != "down":
            last_direction = "down"
        elif decoded_data.startswith("LEFT") and last_direction != "left":
            last_direction = "left"
        elif decoded_data.startswith("RIGHT") and last_direction != "right":
            last_direction = "right"

        # Handle direction change
        if last_direction:
            keyboard.press_and_release(last_direction)

        if decoded_data.lower() == "exit":
            break

    # Disconnect
    gui.sock.close()
    print("Disconnected.")
    gui.append_log("Disconnected.")

if __name__ == "__main__":
    gui = BluetoothGUI()
    gui.root.mainloop()
