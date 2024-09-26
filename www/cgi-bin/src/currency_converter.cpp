#include <iostream>
#include <string>
#include <cstdlib>
#include <iomanip>

using namespace std;

double convertCurrency(double amount, double rate) {
    return amount * rate;
}

int main() {
    cout << "HTTP/1.1 200 OK\r\n";
    cout << "Content-type: text/html\r\n\r\n";
    
    // Exemplo de taxas de conversão fictícias
    const double usdToEur = 0.85;
    const double usdToGbp = 0.75;
    
    // Recupera o valor e a moeda da string de consulta
    string query = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";
    double amount = 0;
    string currency = "EUR";
    
    // Parse do valor do input
    size_t posAmount = query.find("amount=");
    size_t posCurrency = query.find("currency=");
    
    if (posAmount != string::npos) {
        amount = atof(query.substr(posAmount + 7, query.find("&", posAmount) - (posAmount + 7)).c_str());
    }

    if (posCurrency != string::npos) {
        currency = query.substr(posCurrency + 9);
    }

    double converted = 0;
    string currencySymbol = "€";  // Símbolo padrão para Euros

    // Converte o valor de acordo com a moeda selecionada
    if (currency == "EUR") {
        converted = convertCurrency(amount, usdToEur);
        currencySymbol = "€";
    } else if (currency == "GBP") {
        converted = convertCurrency(amount, usdToGbp);
        currencySymbol = "£";
    }

    // HTML gerado
    cout << "<!DOCTYPE html>\n";
    cout << "<html lang=\"en\">\n";
    cout << "<head>\n";
    cout << "    <meta charset=\"UTF-8\">\n";
    cout << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    cout << "    <link rel=\"stylesheet\" href=\"/assets/css/style.css\">\n";
    cout << "    <link rel=\"stylesheet\" href=\"/assets/css/navbar.css\">\n";
    cout << "    <link rel=\"stylesheet\" href=\"/assets/css/banner.css\">\n";
    cout << "    <title>Currency Converter</title>\n";
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
    cout << "        <h1>Currency Converter</h1>\n";
    cout << "        <form method=\"GET\" action=\"/cgi-bin/currency_converter.cgi\">\n";
    cout << "            <label for=\"amount\">Enter Amount in USD:</label><br>\n";
    cout << "            <input type=\"number\" id=\"amount\" name=\"amount\" placeholder=\"Amount in USD\" required><br><br>\n";
    cout << "            <label for=\"currency\">Convert to:</label><br>\n";
    cout << "            <select id=\"currency\" name=\"currency\">\n";
    cout << "                <option value=\"EUR\">EUR (Euros)</option>\n";
    cout << "                <option value=\"GBP\">GBP (Pounds)</option>\n";
    cout << "            </select><br><br>\n";
    cout << "            <input type=\"submit\" value=\"Convert\">\n";
    cout << "        </form>\n";
    
    // Exibe o resultado da conversão se houver um valor
    if (amount > 0) {
        cout << "        <h2>Conversion Result:</h2>\n";
        cout << "        <p>$" << fixed << setprecision(2) << amount << " USD = " << currencySymbol << fixed << setprecision(2) << converted << " " << currency << "</p>\n";
    }

    cout << "    </div>\n";
    cout << "    <script src=\"/assets/js/script.js\"></script>\n";
    cout << "</body>\n";
    cout << "</html>\n";

    return 0;
}
