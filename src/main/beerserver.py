import os
import serial
import serial.tools.list_ports

class Receiver:
    """Blocking call"""
    def start_serial(self, port=None):
        if port is None:
            port = self.get_arduino()

        print("Beer Machine detected at: " + str(port[0]))
        self._serial = serial.Serial(port[0], baudrate=9600, timeout=2)

    """Blocking call"""
    def get_arduino(self):
        def find_arduino():
            for p in serial.tools.list_ports.comports():
                if 'cdc' in p.description.lower():
                    return p
            return None

        arduino_port = find_arduino()
        while not arduino_port:
            arduino_port = find_arduino()

        return arduino_port

    """Blocking call"""
    def recv_line(self):
        while True:
            try:
                return self._serial.readline().decode("ascii")
            except serial.SerialException:
                print("No commands received!")
                if not self._serial.is_open:
                    print("Beer Machine disconnected! Reconnecting... ")
                    self.start_serial()

    """Blocking call"""
    def send(self, message):
        while True:
            try:
                print("Trying to send: " + message);
                return self._serial.write(message.encode("ascii"))
            except serial.SerialTimeoutException:
                print("Can't send string! Trying again!")
                if not self._serial.is_open:
                    print("Beer Machine disconnected! Reconnecting... ")
                    self.start_serial()

def main(cards_file_name="cards.csv", backup_extension=".beerkp"):
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    if os.path.exists(cards_file_name+backup_extension):
        if os.path.exists(cards_file_name):
            bkp_time = os.path.getmtime(cards_file_name+backup)
            original_time = os.path.getmtime(cards_file_name)
            if bkp_time > original_time:
                os.rename(cards_file_name+backup_extension, cards_file_name)
        else:
            os.rename(cards_file_name+backup_extension, cards_file_name)

    if not os.path.exists(cards_file_name):
        with open(cards_file_name, 'w'): pass

    receiver = Receiver()
    print("Initializing connection...")
    receiver.start_serial()
    print("Done initializing, starting operations!")
    while True:
        command = receiver.recv_line()
        print("Received: " + command)
        command = command.split(',')

        # Format: "'check_UUID',(string)UUID"
        if command[0] == "check_UUID":
            found = False
            with open("cards.csv", 'r', encoding='ascii') as cards_file:
                for line in cards_file:
                    UUID = line.split(',')[0]
                    if UUID.strip() == command[1].strip():
                        receiver.send("y")
                        found = True
                        break

            if not found:
                receiver.send("n")

        # Format: "'add_ml',(string)UUID,(int)ml"
        elif command[0] == "add_ml":
            lines = []
            with open(cards_file_name, 'r') as cards_file:
                lines = cards_file.readlines()
            
            new_lines = []
            for line in lines:
                if (line.split(',')[0] == command[1]):
                    new_lines.append(','.join((command[1], str(int(line.split(',')[1])+int(command[2])))))
                    print(new_lines[-1])
                else:
                    new_lines.append(line)

            with open(cards_file_name+backup_extension, 'w') as cards_file:
                cards_file.writelines(new_lines)
            
            os.rename(cards_file_name+backup_extension, cards_file_name)

if __name__ == "__main__":
    main()
