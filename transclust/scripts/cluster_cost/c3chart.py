import sys

for line in sys.stdin:
	data = line

print('''
<!doctype html>

<html lang="en">
<head>
<meta charset="utf-8">

<title>The HTML5 Herald</title>
<meta name="description" content="The HTML5 Herald">
<meta name="author" content="SitePoint">

<link rel="stylesheet" href="c3/c3.css">

<!--[if lt IE 9]>
	<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
<![endif]-->
</head>

<body>
<div id="chart"></div>
<script src="https://d3js.org/d3.v3.min.js" charset="utf-8"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/c3/0.4.11/c3.min.js"></script>

<script type="text/javascript">
var chart = c3.generate({
bindto: '#chart',
data: '''+data+''',"zoom":{enabled:true,rescale:true}});

</script>
</body>
</html>
''')
