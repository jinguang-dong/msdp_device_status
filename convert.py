import pandas as pd
data = pd.read_csv("./code-review.csv", encoding = "utf-8")
print(data)
data.to_excel("./code_review.xlsx", encoding="gb2312")
print("Complete")

