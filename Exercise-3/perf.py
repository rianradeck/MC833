from subprocess import Popen, check_output, DEVNULL
from time import sleep

MAX_CLIENTS = 200


for backlog in range(0, 16):
    print(f"Running with backlog={backlog}")
    sleep(1)
    servproc = Popen(["./servidor", "1234", f"{backlog}"], stdout=DEVNULL)
    
    procs = [ Popen(["./cliente", "127.0.0.1", "1234"], stdout=DEVNULL) for _ in range(MAX_CLIENTS) ]
    
    sleep(10)

    x = str(check_output("netstat -taulpn".split(), stderr=DEVNULL)).split("\\n")
    process = []
    for a in x:
        # print(x[i].split())
        s = a.split()
        if len(s) > 4 and "1234" in s[4]:
            process.append(a)

    estab = 0
    syn = 0
    for p in process:
        # print("PROCESS:", p)
        if "ESTABLISHED" in p:
            estab += 1
        if "SYN_SENT" in p:
            syn += 1

    print(f"With backlog={backlog}, {estab} were ESTABILISHED and {syn} were SYN_SENT")

    for p in procs:
         p.kill()

    servproc.kill()

