import subprocess

def get_cpu_mhz():
    output = subprocess.run(['cat', '/proc/cpuinfo'], capture_output=True, text = True)

    mhz_list = []
    for line in output.stdout.splitlines():
        if 'cpu MHz' in line:
            mhz = float(line.split(':')[1].strip())
            mhz_list.append(mhz)

    if mhz_list:
        return sum(mhz_list)/len(mhz_list)
    else:
        return None
