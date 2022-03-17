# Run this app with `python app.py` and
# visit http://127.0.0.1:8050/ in your web browser.
#coding: utf-8

from time import time
from dash import Dash
import dash_html_components as html
import dash_core_components as dcc
import plotly.express as px
import pandas as pd
import numpy as np
# import matplotlib
import matplotlib.pyplot as plt


# app = Dash(__name__)

# # assume you have a "long-form" data frame
# # see https://plotly.com/python/px-arguments/ for more options
# df = pd.DataFrame({
#     "Fruit": ["Apples", "Oranges", "Bananas", "Apples", "Oranges", "Bananas"],
#     "Amount": [4, 1, 2, 2, 4, 5],
#     "City": ["SF", "SF", "SF", "Montreal", "Montreal", "Montreal"]
# })

# fig = px.bar(df, x="Fruit", y="Amount", color="City", barmode="group")

# app.layout = html.Div(children=[
#     html.H1(children='Hello Dash'),

#     html.Div(children='''
#         Dash:  web application framework for your data.
#     '''),

#     dcc.Graph(
#         id='example-graph',
#         figure=fig
#     )
# ])
# if __name__ == '__main__':
#     app.run_server(debug=True)

plt.rcParams['font.sans-serif'] = 'simhei'#用来显示中文标签
plt.rcParams['axes.unicode_minus']=False
 
#------------------------------------------------------
data = pd.read_csv("data.csv",header=None)
data = np.array(data) #转为列表
last = data.shape[0] - 1 #总行数减一，因为从0开始
# data = data0.loc[[last],:]
print(last)
print(data)
 
labels=['wrongPosition','goodPosition']
#绘图显示的标签
values=data[last,[0,1]]#获取最后一行，第0列和第1列的地址
print(values)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
# 分析坐姿数据
#plt.title("Proportion of correct and incorrect sitting posture",fontsize=15)
plt.title("一段时间内坐姿概览",fontsize=15)
plt.pie(values,labels=labels,autopct="%.1f%%")
# plt.show()
plt.savefig('./analyseResult/一段时间内坐姿概览.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠

#----------------------------------------------------------------------------
data1 = pd.read_csv("neck.csv",header=None)
data1 = np.array(data1)
last1 = data1.shape[0] - 1 
# data = data.loc[[last1],:]
print(data1)
print(last1)
 
labels1=['wrongNeck','goodNeck']
#绘图显示的标签
# values1=data1[last1,[0,1]]#获取最后一行，第0列和第1列的地址
values1=data1[last1,[1,2]]#获取最后一行，第1列和第2列的地址
print(values1)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
#分析坐姿数据
#plt.title("Proportion of correct and incorrect neck posture",fontsize=15)
plt.title("一段时间内颈部位置姿势概览",fontsize=15)
plt.pie(values1,labels=labels1,autopct="%.1f%%")
# plt.show()
plt.savefig('./analyseResult/一段时间内颈部位置姿势概览.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠

#----------------------------------------------------------------------------
data2 = pd.read_csv("shoulder.csv",header=None)
data2 = np.array(data2)
last2 = data2.shape[0]  - 1 #总行数减一
# data = data.loc[[last2],:]
print(data2)
print(last2)
 
labels2=['wrongShoulder','goodShoulder']
#绘图显示的标签
# values2=data2[last2,[0,1]]#获取最后一行，第0列和第1列的地址
values2=data2[last2,[1,2]]#获取最后一行，第1列和第2列的地址
print(values2)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
#分析坐姿数据
#plt.title("Proportion of correct and incorrect shoulder posture",fontsize=15)
plt.title("一段时间内肩膀位置姿势概览",fontsize=15)
plt.pie(values2,labels=labels2,autopct="%.1f%%")
# plt.show()
plt.savefig('./analyseResult/一段时间内肩膀位置姿势概览.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠

#----------------------------------------------------------------------------
#绘制曲线图
data3 = pd.read_csv("total.csv",header=None)#不将第一行作为列名
data3 = np.array(data3)
total_rows = data3.shape[0] #总行数
print(data3)
#取第一列,正误数组
list1= data3[:,0]
print(list1)
#取第三列作为时间数组
time1 = data3[:,2]
print(time1)
#设置标题
plt.title('坐姿随时间变化1表示正确，0表示错误')
#画出折线图
plt.xlabel('Time')
plt.ylabel('Change')
plt.plot(time1,list1)
# plt.show()
plt.savefig('./analyseResult/坐姿随时间变化图.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠








# 导入tailwindcss的CDN
external_script = ["https://tailwindcss.com/", {"src": "https://cdn.tailwindcss.com"}]
 
# 创建Dash实例
app = Dash(
    __name__,
    external_scripts=external_script,
)
app.scripts.config.serve_locally = True

# 创建数据
df = pd.DataFrame(
    {
        # "Fruit": ["果", "橙子", "香蕉", "苹果", "橙子", "香蕉"],
        # "Amount": [4.2, 1.0, 2.1, 2.32, 4.20, 5.0],
        # "City": ["北京", "北京", "北京", "上海", "上海", "上海"],
        "status" : list1, #折线的纵轴，0/1
        "time" : time1, #折线的横轴，时间
    }
) 
# df1 = pd.DataFrame.from_dict(
#     {
#         "badSituation":df.status.sum(),
#         "goodSituation":total_rows-df.status.sum()
#     },
#     orient='index'
# )


# # 水果单数
# fruit_count = df.Fruit.count()
# # 销售总额
# total_amt = df.Amount.sum()
# # 城市单数
# city_count = df.City.count()
# 变量数
# variables = df.shape[1]
#总时间
total_time = df.time.sum() / 60.0 #分钟为单位
#正确坐姿占比
proportion_right_total = (df.status.sum() / total_rows) * 100 #正确次数除以
#肩膀正确率
proportion_right_shoulder = values2[0] / values2[1] * 100
#脖子正确率
proportion_right_neck = values1[0] / values1[1] * 100

badSituation = df.status.sum()
goodSituation = total_rows-df.status.sum()

# 柱状图1, 不同水果不同城市的销售额
# fig = px.bar(df, x="Fruit", y="Amount", color="City", barmode="group")
# 箱型图1, 不同城市的销售额分布情况
# fig1 = px.box(df, x="City", y="Amount", color="City")
#折线图
# fig = px.line(df, x="Amount", y="Fruit", color="City")
# fig1 = px.pie(df, values="Amount", names="Fruit", title="水果销售额分布情况")
fig = px.scatter(df, x="time", y="status", color="status",title="坐姿波动情况")
fig.write_image('analyseResult/坐姿波动情况.png')
#饼图
fig1 = px.pie(labels=['wrongPosition','goodPosition'],values=[badSituation,goodSituation],title="坐姿正确率",color_discrete_map={'wrongPosition':'red','goodPosition':'green'})
fig1.write_image('analyseResult/坐姿正确率.png')
app.layout = html.Div(
    html.Div(
        children=[
            html.Div(
                children=[
                    html.H1(children="坐姿检测--可视化报表", className=" py-3 text-5xl font-bold text-blue-800"),
                    html.Div(
                        # children="""Python with Dash = 💝 .""",
                        children="""正确率大于 70% 即为合格，请继续保持，否则请根据结果进行调整""",
                        className="text-left prose prose-lg text-2xl  py-3 text-gray-600",
                    ),
                ],
                className="w-full mx-14 px-16 shadow-lg bg-white -mt-14 px-6 container my-3 ",
            ),
            html.Div(
                html.Div(
                    children=[
                        # html.Div(
                        #     children=[
                        #         f"¥{total_amt}",
                        #         html.Br(),
                        #         html.Span("总销售额", className="text-lg font-bold ml-4"),
                        #     ],
                        #     className=" shadow-xl py-4 px-14 text-5xl bg-[#76c893] text-white  font-bold text-gray-800",
                        # ),
                        html.Div(
                        children=[
                            #保留两位小数
                            f"{total_time:.2f}min",
                            html.Br(),
                            html.Span("总时间", className="text-lg font-bold ml-4"),
                        ],
                        className=" shadow-xl py-4 px-14 text-5xl bg-[#76c893] text-white  font-bold text-gray-800",
                        ),
                        html.Div(
                        children=[
                            f"{proportion_right_neck:.2f}%",
                            html.Br(),
                            html.Span("脖子正确率", className="inline-flex items-center text-lg font-bold ml-4"),
                        ],
                        className=" shadow-xl py-4 px-24 text-5xl bg-[#646ffa] text-white  font-bold text-gray-800",
                        ),
                        html.Div(
                        children=[
                            f"{proportion_right_shoulder:.2f}%",
                            html.Br(),
                            html.Span("肩膀正确率", className="inline-flex items-center text-lg font-bold ml-4"),
                            ],
                            className=" shadow-xl py-4 px-24 text-5xl bg-[#646ffa] text-white  font-bold text-gray-800",
                        ),
                        html.Div(
                            children=[
                                f"{proportion_right_total:.2f}%",
                                html.Br(),
                                html.Span("总正确率", className="text-lg font-bold ml-4"),
                            ],
                            className=" shadow-xl py-4 px-24 text-5xl bg-[#1d3557] text-white  font-bold text-gray-800",
                        ),
                        # html.Div(
                        #     children=[
                        #         # f"{proportion_right_total:.2f}%",
                        #         html.Br(),
                        #         #打印一段文字
                        #         html.Span("折线图", className="text-lg font-bold ml-4"),
                        #         html.Span("总正确率", className="text-lg font-bold ml-4"),
                        #     ],
                        #     className=" shadow-xl py-4 px-24 text-5xl bg-[#1d3557] text-white  font-bold text-gray-800",
                        # ),
                        # html.Div(
                        #     children=[
                        #         variables,
                        #         html.Br(),
                        #         html.Span("变量", className="inline-flex items-center text-lg font-bold ml-4"),
                        #     ],
                        #     className=" shadow-xl py-4 px-24 text-5xl bg-[#646ffa] text-white  font-bold text-gray-800",
                        # ),
                        # html.Div(
                        #     children=[
                        #         city_count,
                        #         html.Br(),
                        #         html.Span("城市数量", className="text-lg font-bold ml-4"),
                        #     ],
                        #     className="w-full shadow-xl py-4 px-24 text-5xl bg-[#ef553b] text-white  font-bold text-gray-800",
                        # ),
                    ],
                    className="my-4 w-full grid grid-flow-rows grid-cols-1 lg:grid-cols-4 gap-y-4 lg:gap-[60px]",
                ),
                className="flex max-w-full justify-between items-center ",
            ),
            html.Div(
                children=[
                    html.Div(
                        children=[
                            dcc.Graph(id="example-graph", figure=fig),
                        ],
                        className="shadow-xl w-full border-3 rounded-sm",
                    ),
                    # html.Div(
                    #     children=[
                    #         dcc.Graph(id="example-graph1", figure=fig1),
                    #     ],
                    #     className="w-full shadow-2xl rounded-sm",
                    # ),
                    #xxj添加一个图片
                    html.Div(
                    children=[
                        dcc.Graph(id="example-graph1", figure=fig1),
                    ],
                        className="shadow-xl w-full border-3 rounded-sm",
                    ),
                ],
                className="grid grid-cols-1 lg:grid-cols-2 gap-4",
            ),
        ],
        className="bg-[#ebeaee]  flex py-14 flex-col items-center justify-center ",
    ),
    className="bg-[#ebeaee] container mx-auto px-14 py-4",
)

if __name__ == '__main__':
    # debug模式, 端口7777
    app.run_server(debug=True, threaded=True, port=7777)
    # 正常模式, 网页右下角的调试按钮将不会出现
    # app.run_server(port=7777)