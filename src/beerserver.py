# Receiver
import os
import serial
import serial.tools.list_ports

# Non blocking read
import sys
import select
import tty
import termios

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
    def recv_line(self, blocking=False):
        while True:
            try:
                return self._serial.readline().decode("ascii")
            except serial.SerialException:
                print("No commands received!")
                if not blocking:
                    return None
                if not self._serial.is_open:
                    print("Beer Machine disconnected! Reconnecting... ")
                    self.start_serial()

    """Blocking call"""
    def send(self, message, blocking=False):
        while True:
            try:
                #print("Trying to send: " + message);
                return self._serial.write(message.encode("ascii"))
            except serial.SerialTimeoutException:
                print("Can't send string! Trying again! Are you sure the machine is connected?")
                if not blocking:
                    return None
                if not self._serial.is_open:
                    print("Beer Machine disconnected! Reconnecting... ")
                    self.start_serial()

class NonBlockingConsole(object):

    def __enter__(self):
        self.old_settings = termios.tcgetattr(sys.stdin)
        tty.setcbreak(sys.stdin.fileno())
        return self

    def __exit__(self, type, value, traceback):
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.old_settings)


    def get_data(self):
        if select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
            return sys.stdin.read(1)
        return False

def get_entry_from_DB(uuid):
    with open("cards.csv", 'r', encoding='ascii') as cards_file:
        for line in cards_file:
            DB_UUID = line.split(',')[0]
            if DB_UUID.strip() == uuid.strip():
                return line

    return None

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

    print("Welcome to beer machine server!")
    print("To enter maintenance mode press 'm' and 'Enter'")
    print("To add a new user press 'u' and 'Enter'")
    print("Everything else just works out of the box! :)\n")

    in_maintenance = False
    adding_user = False

    receiver = Receiver()
    print("Initializing connection...")
    receiver.start_serial()
    print("Done initializing, starting operations!")
    while True:
        command = None
        mode = False
        with NonBlockingConsole() as nbc:
            command = receiver.recv_line()
            mode = nbc.get_data()

        if not mode and command is None:
            continue
        elif mode == "m":
            in_maintenance = True
        elif mode == "u":
            adding_user = True

        command = command.split(',')
        command = [x.strip() for x in command]

        # Format: "'check_UUID',(string)UUID"
        if command[0] == "check_UUID":
            entry = get_entry_from_DB(command[1])
            if entry:
                print(entry.split(',')[2].strip().title() + "'s card with " + entry.split(',')[1] + "ml consumed found!")
                receiver.send("y")
            else:
                receiver.send("n")
        
        # Format: "'check_maintenance'"
        elif command[0] == "check_maintenance":
            if in_maintenance:
                receiver.send("y")
                in_maintenance = False
            else:
                receiver.send("n")

        # Format: "'check_adding_user'"
        elif command[0] == "check_adding_user":
            if adding_user:
                receiver.send("y")
                print("Please insert card!")
                line = receiver.recv_line()
                user_command = line.split(',')[0]
                UUID = line.split(',')[1]
                while user_command != "raw_UUID":
                    if user_command == "check_adding_user":
                        receiver.send("y")
                    else:
                        receiver.send("n")
                    line = receiver.recv_line()
                    user_command = line.split(',')[0]
                    UUID = line.split(',')[1]
                if get_entry_from_DB(UUID) is not None:
                    print("Please enter a new card!")
                else:
                    name = input("Enter new user name: ")
                    with open(cards_file_name, "a") as cards_file:
                        cards_file.write(UUID.strip()+",0,"+name.strip()+'\n')
                    print("Now press the machine button to end adding!")
                    adding_user = False
            else:
                receiver.send("n")

        # Format: "'add_ml',(string)UUID,(int)ml"
        elif command[0] == "add_ml":
            lines = []
            with open(cards_file_name, 'r') as cards_file:
                lines = cards_file.readlines()
            
            new_lines = []
            for line in lines:
                if (line.split(',')[0] == command[1]):
                    new_lines.append(','.join((command[1], str(int(line.split(',')[1])+int(command[2])), line.split(',')[2])))
                else:
                    new_lines.append(line)

            with open(cards_file_name+backup_extension, 'w') as cards_file:
                cards_file.writelines(new_lines)
            
            os.rename(cards_file_name+backup_extension, cards_file_name)

        # Format: "'get_name',(string)UUID"
        elif command[0] == "get_name":
            entry = get_entry_from_DB(uuid)
            if entry is not None:
                receiver.send(entry.split(',')[2]+'\n')
            else:
                receiver.send("\n")

        # Format: "'get_ml',(string)UUID"
        elif command[0] == "get_ml":
            entry = get_entry_from_DB(uuid)
            if entry is not None:
                receiver.send(entry.split(',')[1]+'\n')
            else:
                receiver.send("\n")

if __name__ == "__main__":
    main()
