import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("Results/Report.csv")
fig, axis = plt.subplots(2, 4)
for i in range(2, 17, 2):
    row = 1 if i > 8 else 0
    col = (i // 2 - 1) % 4
    axis[row][col].plot("Input size", "Serial", data=df, marker="o", label="Serial")
    axis[row][col].plot("Input size", f"{i}-Naive", data=df, marker="o", label=f"{i}-Naive", linestyle="--")
    axis[row][col].plot("Input size", f"{i}-OMP", data=df, marker="o", label=f"{i}-OMP", linestyle=":")
    axis[row][col].set(title=f"Threads: {i}", xlabel="Input size", ylabel="Time taken (microseconds)")
    axis[row][col].legend()
plt.show()
