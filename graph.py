import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import datetime
#Topic    Date    Time    Amps

# Calling DataFrame constructor on list
df = pd.read_csv('meduk2-capstone-data1.csv')
df['datetime'] = pd.to_datetime(df['Date'] + ' ' + df['Time'])

#Working#######################################################################################
#will give hourly average
'''df.groupby([df['datetime'].dt.hour, 'Date'])['Amps'].mean().to_csv('hourly_averages.csv')
hourly_avg = pd.read_csv('hourly_averages.csv')
df_filtered = hourly_avg.groupby(hourly_avg['datetime'])['Amps'].mean()
df_filtered.plot(x='datetime', y='Amps', kind='bar')
plt.title("Average Energy Use By Hour of Day")
plt.xlabel("Hour of Day")
plt.ylabel("Amps")
plt.tight_layout()
plt.savefig('hourly_avg.png')'''

#Gives averages by day
'''df.groupby([df['datetime'].dt.day, 'Date'])['Amps'].mean().to_csv('daily_averages.csv')
daily_avg = df.groupby([df['datetime'].dt.day, 'Date'])['Amps'].mean()
daily_avg.plot(x='Date', y='Amps', kind='bar')
plt.title("Average Energy Use By Date")
plt.xlabel("Date")
plt.ylabel("Amps")
plt.tight_layout()
plt.savefig('daily_avg.png')'''

#Gives today's average use
#x = datetime.datetime.now()
#todays_date = x.strftime("%-m/%d/%Y")
#pivot = df[df['Date'] == todays_date]
#todays_avg = pivot.groupby([pivot['datetime'].dt.day, 'Date'])['Amps'].mean().round(2).reset_index()
#value = todays_avg['Amps'].iloc[0]
#print(value)
##################################################################################################




