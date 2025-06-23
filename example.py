import multiprocessing
import time
import sharedfile

def writer():
    with sharedfile.open("shared_test.txt", mode="r", share="rw") as f:
        for i in range(5):
            line = f"Writer line {i}\n"
            f.write(line)
            f.flush()
            print(f"[Writer] wrote: {line.strip()}")
            time.sleep(1)

def reader():
    time.sleep(0.5)  # Let writer start first
    with sharedfile.open("shared_test.txt", mode="r", share="rw") as f:
        for _ in range(5):
            f.seek(0)
            content = f.read()
            print(f"[Reader] read:\n{content.strip()}")
            time.sleep(1)

if __name__ == "__main__":
    # Spawn two separate processes
    p1 = multiprocessing.Process(target=writer)
    p2 = multiprocessing.Process(target=reader)

    p1.start()
    p2.start()

    p1.join()
    p2.join()
