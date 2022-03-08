import pandas as pd
import numpy as np
# import matplotlib
import matplotlib.pyplot as plt
# import csv
plt.rcParams['font.sans-serif'] = 'simhei'
plt.rcParams['axes.unicode_minus']=False
 

data = pd.read_csv("data.csv")
data = np.array(data)
last = data.shape[0] - 1#总行数减一
# data = data.loc[[last],:]
print(data)
 
labels=['wrong','good']
#绘图显示的标签
values=data[last,[0,1]]#获取最后一行，第0列和第1列的地址
print(values)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
# 分析坐姿数据
plt.title("Proportion of correct and incorrect sitting posture",fontsize=15)

plt.pie(values,labels=labels,autopct="%.1f%%")
plt.show()

data1 = pd.read_csv("neck.csv")
data = np.array(data1)
last = data.shape[0] - 1#总行数减一
# data = data.loc[[last],:]
print(data)
 
labels=['wrongNeck','goodNeck']
#绘图显示的标签
values1=data[last,[0,1]]#获取最后一行，第0列和第1列的地址
print(values1)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
#分析坐姿数据
plt.title("Proportion of correct and incorrect neck posture",fontsize=15)

plt.pie(values,labels=labels,autopct="%.1f%%")
plt.show()