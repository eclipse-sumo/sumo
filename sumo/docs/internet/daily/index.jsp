<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
        <head>
                <title>Delphi - Status of daily build and test</title>
        </head>
        <body>
<center>
        <h2>Status of daily build and test</h2>
</center>
<table>
<%@ page import="java.io.*" %>
<%
int column = 0;
String[] rows = {"<th></th><th>", "<th>Standard build</th><td>",
                 "<th>Texttest tests</th><td>", "<th>Debug build</th><td>"};
for (File f : (new File("webapps/daily")).listFiles()) {
    String name = f.getName();
    if (name.endsWith("status.log")) {
        String prefix = name.substring(0, name.indexOf("status.log"));
        int rowIndex = 0;
        BufferedReader reader = new BufferedReader(new FileReader(f));
        String line = reader.readLine();
        boolean hadLinkLine = false;
        while (line != null) {
            if (line.trim().equals("--")) {
                if (hadLinkLine) {
                    rows[rowIndex] += "</pre></a>";
                }
                if (rowIndex == 0) {
                    rows[rowIndex] += "</th><th>";
                } else {
                    rows[rowIndex] += "</td><td>";
                }
                rowIndex++;
                hadLinkLine = false;
            } else  {
                if (line.startsWith("batchreport")) {
                    String dir = line.split(" ")[0].substring(12);
                    rows[rowIndex] += "<a href=\"" + prefix + "report/" + dir + "/test_default.html\">";
                    rows[rowIndex] += line.substring(12) + "</a><br/>";
                } else {
                    if (hadLinkLine || rowIndex == 0) {
                        rows[rowIndex] += line + "\n";
                    } else {
                        rows[rowIndex] += "<a href=\"" + line + "\"><pre>";
                        hadLinkLine = true;
                    }
                }
            }
            line = reader.readLine();
        }
    }
}
for (int i = 0; i < rows.length; i++) {
    if (i == 0) {
        rows[i] += "</th>";
    } else  {
        rows[i] += "</td>";
    }
%>
<tr>
<%= rows[i] %>
</tr>
<%
}
%>
</table>

        </body>
</html>

