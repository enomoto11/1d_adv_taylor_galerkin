import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("example/out_taylor.dat", dtype=float)
ITERATED_TIME = 7

length = []
for i in range(101):
    length.append(i * 0.01)

index = [f"t={10 * i}[s]" for i in range(ITERATED_TIME)]

fig = plt.figure()  # define the name for saving png file
plt.rcParams["font.family"] = "sans-serif"  # 使用するフォント
# x軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams["xtick.direction"] = "in"
# y軸の目盛線が内向き('in')か外向き('out')か双方向か('inout')
plt.rcParams["ytick.direction"] = "in"
plt.rcParams["xtick.major.width"] = 1.0  # x軸主目盛り線の線幅
plt.rcParams["ytick.major.width"] = 1.0  # y軸主目盛り線の線幅
plt.rcParams["font.size"] = 12  # フォントの大きさ
plt.rcParams["axes.linewidth"] = 1.0  # 軸の線幅edge linewidth。囲みの太さ
# ↓グラフ位置の調整
plt.subplots_adjust(
    left=0.2, bottom=0.2, right=None, top=0.97, wspace=None, hspace=None
)


for i in range(ITERATED_TIME):
    plt.plot(length, data[i])

plt.xlabel("Length[m]")
plt.ylabel("f")

# Legend
plt.legend(
    index,
    bbox_to_anchor=(
        0.5,
        -0.1,
    ),
    loc="upper center",
    borderaxespad=0.5,
    ncol=4,
    fontsize=10,
)

fig.savefig("result_png/out_taylor.png")  # save png file
