import subprocess
from concurrent.futures import ThreadPoolExecutor

# simulate the execution of the script
def run_script(param):
    command = ["./o_solution", "global", str(param), "global/GlobalTest.csv", "global/LUTTest.csv"]
    with open('results.txt', 'a') as f:
        subprocess.run(command, stdout=f)

with ThreadPoolExecutor(max_workers=3) as executor: # change max_workers to specific number of threads
    for _ in range(10): # number of times of execution
        executor.submit(run_script, 3) # modify parameter