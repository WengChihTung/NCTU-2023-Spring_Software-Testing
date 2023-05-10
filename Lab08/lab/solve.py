import angr
import sys

main_addr = 0x4011a9
find_addr = 0x401380
avoid_addr = 0x401385

def handle_fgets_real_input(raw_input):
    idx = 0
    for c in raw_input:
        if c == ord('\n') or c == ord('\0'):
            break
        idx += 1
    return raw_input[:idx]

class my_fgets(angr.SimProcedure):
    def run(self, fmt, n):
        simfd = self.state.posix.get_fd(sys.stdin.fileno())
        data, ret_size = simfd.read_data(4)
        self.state.memory.store(n, data)
        return 1

proj = angr.Project('./src/prog', load_options={'auto_load_libs': False})
proj.hook_symbol('__isoc99_scanf', my_fgets(), replace=True)

state = proj.factory.blank_state(addr=main_addr)

simgr = proj.factory.simulation_manager(state)
simgr.explore(find=find_addr, avoid=avoid_addr)
if simgr.found:
    print(simgr.found[0].posix.dumps(sys.stdin.fileno()))

    s = simgr.found[0].posix.dumps(sys.stdin.fileno())
    f = open("solve_input", "w")
    for i in range(0, 15):
        ans = int.from_bytes(s[i * 4 : i * 4 + 4], byteorder='little')
        print(ans)
        f.write(str(ans) + "\n")
    f.close()
else:
    print('Failed')
