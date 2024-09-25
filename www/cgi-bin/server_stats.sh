#!/bin/bash

# Define o cabeçalho HTTP para conteúdo HTML
echo -e "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
echo ""

# Coletando estatísticas do sistema
cpu_usage=$(top -l 1 | grep "CPU usage" | awk '{print $3}' | sed 's/%//')  # Uso de CPU (sem a parte decimal)
storage_free=$(df / | awk 'NR==2 {print $4}')  # Espaço livre (em blocos)
storage_used=$(df / | awk 'NR==2 {print $3}')  # Espaço usado (em blocos)
disk_usage=$(df -h / | awk 'NR==2 {print $5}' | sed 's/%//')  # Uso de Disco (percentual)
uptime=$(uptime | sed 's/^.*up \([^,]*\), .*$/\1/')  # Tempo de Atividade

# Cálculos de armazenamento (em %)
total_storage=$(($storage_free + $storage_used))
storage_used_percentage=$(echo "scale=2; $storage_used / $total_storage * 100" | bc)  # Calcula o uso de armazenamento em %

# Função para criar um gráfico de pizza em SVG
create_pie_chart() {
    local usage=$1
    local total=100
    local remaining=$(echo "$total - $usage" | bc)

    local angle=$(echo "$usage * 3.6" | bc)  # Calcula o ângulo para o uso
    local large_arc=0
    if (( $(echo "$usage > 50" | bc -l) )); then
        large_arc=1
    fi

    echo "<svg width='200' height='200' viewBox='0 0 32 32'>"
    echo "  <circle r='16' cx='16' cy='16' fill='#eee'></circle>"  # Fundo cinza claro
    echo "  <path d='M 16,16 L 16,0 A 16,16 0 $large_arc,1 $(echo "16+16*s($angle/57.2958)" | bc -l),$(echo "16-16*c($angle/57.2958)" | bc -l) z' fill='blue'></path>"  # Parte do gráfico preenchida
    echo "  <text x='16' y='16' font-size='3' text-anchor='middle' dy='.3em' fill='black'>$usage%</text>"  # Exibe a porcentagem
    echo "</svg>"
}

# HTML para exibir as estatísticas com gráficos de pizza
cat <<EOT
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="/assets/css/style.css">
    <link rel="stylesheet" href="/assets/css/navbar.css">
    <link rel="stylesheet" href="/assets/css/banner.css">
    <title>Server Statistics</title>
    <style>
        .stats-container {
            display: flex;
            justify-content: space-around;
            align-items: center;
            margin-top: 50px;
        }
        .stats-item {
            text-align: center;
        }
        svg {
            margin-top: 20px;
        }
    </style>
</head>

<body>
    <div class="banner"></div>
    <div id="mySidenav" class="sidenav">
        <div style="padding-bottom: 50px;">
            <a id="closebtn" href="javascript:void(0)" class="closebtn" onclick="closeNav()">&times;</a>
        </div>
        <div class="pages">
            <a href="index.html">Home</a>
            <a href="server_architecture.html">Server Architecture</a>
            <a href="http_protocol.html">Understanding HTTP</a>
            <a href="socket.html">Socket Programming</a>
            <a href="request_handling.html">Request Handling</a>
            <a href="cgi_handler.html">CGI Implementation</a>
            <a href="logging.html">Logging and Monitoring</a>
        </div>
    </div>

    <span id="openNav" onclick="openNav()">&#9776;</span>

    <div id="main">
        <h1 style="text-align:center;">Server Statistics</h1>
        <div class="stats-container">
            <div class="stats-item">
                <h2>CPU Usage</h2>
                $(create_pie_chart $cpu_usage)
            </div>
            <div class="stats-item">
                <h2>Storage Usage</h2>
                $(create_pie_chart $(printf "%.0f" $storage_used_percentage))  <!-- Adaptando o valor do armazenamento -->
            </div>
            <div class="stats-item">
                <h2>Disk Usage</h2>
                $(create_pie_chart $disk_usage)
            </div>
        </div>
        <div style="text-align: center; margin-top: 40px;">
            <h2>Uptime</h2>
            <p style="display: inline-block;">$uptime</p>  <!-- Centralizando o parágrafo -->
        </div>
    </div>

    <script src="/assets/js/script.js"></script>
</body>

</html>
EOT
