import plotly.offline as py
import plotly.graph_objs as go
import numpy as np

x, y, z = np.random.multivariate_normal(np.array([0,0,0]), np.eye(3), 200).transpose()
print(x)
trace1 = go.Scatter3d(
    x=x,
    y=y,
    z=z,
    mode='markers',
    marker=dict(
        size=5,
        line=dict(
            color='rgba(217, 217, 217, 0.14)',
            width=0.5
        ),
        opacity=0.8
    )
)
data = [trace1]
layout = go.Layout(
		margin=dict(
			l=0,
			r=0,
			b=0,
			t=0
			)
		)
fig = go.Figure(data=data, layout=layout)
py.plot(fig, filename='simple-3d-scatter')
