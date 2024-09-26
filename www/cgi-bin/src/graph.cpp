#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

void printSVG(const vector<int>& values) {
    // Definindo tamanhos básicos do gráfico
    const int width = 500;
    const int height = 300;
    const int barWidth = width / values.size();  // Calcula a largura de cada barra
    const int maxBarHeight = height - 20;  // Altura máxima para as barras

    cout << "<svg width='" << width << "' height='" << height << "' xmlns='http://www.w3.org/2000/svg'>";

    for (size_t i = 0; i < values.size(); ++i) {
        int barHeight = (values[i] * maxBarHeight) / 100;  // Normaliza os valores para 100%
        int x = i * barWidth;
        int y = height - barHeight;

        // Desenha a barra
        cout << "<rect x='" << x << "' y='" << y << "' width='" << barWidth - 5 << "' height='" << barHeight << "' fill='blue' />";
        cout << "<text x='" << (x + barWidth / 4) << "' y='" << (height - 5) << "' font-size='12' fill='black'>" << values[i] << "</text>";
    }

    cout << "</svg>";
}

int main() {
    cout << "HTTP/1.1 200 OK\r\n";
    cout << "Content-type: text/html\r\n\r\n";

    // Exibindo formulário para entrada de dados
    cout << "<!DOCTYPE html>\n";
    cout << "<html lang=\"en\">\n";
    cout << "<head>\n";
    cout << "    <meta charset=\"UTF-8\">\n";
    cout << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    cout << "    <link rel=\"stylesheet\" href=\"/assets/css/style.css\">\n";
    cout << "    <link rel=\"stylesheet\" href=\"/assets/css/navbar.css\">\n";
    cout << "    <link rel=\"stylesheet\" href=\"/assets/css/banner.css\">\n";
    cout << "    <title>Bar Chart Generator</title>\n";
    cout << "</head>\n";
    cout << "<body>\n";
    cout << "    <div class=\"banner\"></div>\n";
    cout << "    <div id=\"mySidenav\" class=\"sidenav\">\n";
    cout << "        <div style=\"padding-bottom: 50px;\">\n";
    cout << "            <a id=\"closebtn\" href=\"javascript:void(0)\" class=\"closebtn\" onclick=\"closeNav()\">&times;</a>\n";
    cout << "        </div>\n";
    cout << "        <div class=\"pages\">\n";
    cout << "            <a href=\"index.html\">Home</a>\n";
    cout << "            <a href=\"server_architecture.html\">Server Architecture</a>\n";
    cout << "            <a href=\"http_protocol.html\">Understanding HTTP</a>\n";
    cout << "            <a href=\"socket.html\">Socket Programming</a>\n";
    cout << "            <a href=\"request_handling.html\">Request Handling</a>\n";
    cout << "            <a href=\"cgi_handler.html\">CGI Implementation</a>\n";
    cout << "            <a href=\"logging.html\">Logging and Monitoring</a>\n";
    cout << "        </div>\n";
    cout << "    </div>\n";
    cout << "    <span id=\"openNav\" onclick=\"openNav()\">&#9776;</span>\n";
    cout << "    <div id=\"main\">\n";
    cout << "        <h1>Enter Values to Generate a Bar Chart</h1>\n";
    cout << "        <form method=\"GET\" action=\"/cgi-bin/bar_chart.cgi\">\n";
    cout << "            <label for=\"values\">Enter values (comma-separated):</label><br>\n";
    cout << "            <input type=\"text\" id=\"values\" name=\"values\" placeholder=\"10,20,30,40\"><br><br>\n";
    cout << "            <input type=\"submit\" value=\"Generate Chart\">\n";
    cout << "        </form>\n";
    cout << "    </div>\n";

    // Parsing and processing the input data
    string query = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";
    if (!query.empty()) {
        size_t pos = query.find("values=");
        if (pos != string::npos) {
            string valuesString = query.substr(pos + 7);
            replace(valuesString.begin(), valuesString.end(), '+', ' ');

            stringstream ss(valuesString);
            string token;
            vector<int> values;

            while (getline(ss, token, ',')) {
                values.push_back(stoi(token));
            }

            if (!values.empty()) {
                // Renderizando o gráfico de barras como SVG
                cout << "<div id='main'><h2>Generated Bar Chart:</h2>";
                printSVG(values);
                cout << "</div>";
            }
        }
    }

    cout << "<script src=\"/assets/js/script.js\"></script>\n";
    cout << "</body>\n";
    cout << "</html>\n";
    
    return 0;
}
