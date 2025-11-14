![Acessly_Banner](https://drive.google.com/uc?export=view&id=1p_fDAFyCxt0pcYdaU24ZLq0ltW9J1Rwj)

## 🎓 Sobre o projeto

O **Acessly Monitor** é um sistema de monitoramento inteligente que avalia em tempo real as condições de acessibilidade em ambientes corporativos. Através de sensores IoT, a solução coleta dados sobre **temperatura**, **luminosidade**, **ruído** e **obstruções**, calculando um **Score de Acessibilidade** que reflete a adequação do ambiente para pessoas com deficiência (PCDs).

## 🎯 Objetivo

Fornecer transparência e dados objetivos sobre as condições reais de acessibilidade em espaços de trabalho, auxiliando empresas a cumprir a **Lei de Cotas (Lei no 8.213/91)** e promover ambientes verdadeiramente inclusivos.

### 🧵 Alinhamento com o tema "O Futuro do Trabalho"

O projeto integra:

- **Digitalização inclusiva**: uso de IoT para monitoramento automatizado
- **Diversidade estratégica**: dados para promover inclusão de PCDs
- **Agenda ESG**: contribuição com `ODS 10 (Redução Das Desigualdades)`
- **Ambientes adaptáveis**: workplaces personalizados e acessíveis

## 🚨 Problemática

### O Desafio da Inclusão no Mercado de Trabalho

O Brasil possui **17,2 milhões** de pessoas com algum tipo de deficiência (8,4% da população). Apesar da legislação, os números revelam:

| Indicador | PCDs | Pessoas sem Deficiência |
|-----------|------|------------------------|
| **Taxa de Participação no Mercado** | 28,3% | 66,3% |
| **Com Carteira Assinada** | 34,3% | 50,9% |
| **Salário Médio** | R$ 1.639 | R$ 2.619 (62% a menos) |
| **Fora do Mercado** | 7 em cada 10 | - |

### Barreiras identificadas

- ❌ Falta de transparência sobre acessibilidade real dos ambientes
- ❌ Ambientes não adaptados às necessidades das PCDs
- ❌ Dificuldade de empresas em encontrar candidatos compatíveis
- ❌ Processos seletivos não inclusivos

## 💡 Solução proposta

O **Acessly Monitor** oferece:

- ✅ **Monitoramento contínuo**: coleta de dados ambientais 24/7  
- ✅ **Score de Acessibilidade**: métrica objetiva (0-100%) calculada em tempo real  
- ✅ **Alertas automáticos**: notificações quando condições ficam inadequadas  
- ✅ **Dashboard Web**: visualização intuitiva e acessível dos dados  
- ✅ **Histórico de dados**: gráficos para análise temporal  
- ✅ **Transparência total**: dados públicos sobre acessibilidade do ambiente 

![Acessly_Monitor](https://drive.google.com/uc?export=view&id=1jXGaZ2s30W61gtRwhKozCgcC3x7wt_lr)

### 🏗️ Arquitetura do sistema

```
┌─────────────────────────────────────────────────────────────┐
│                      ARQUITETURA IoT                        │
└─────────────────────────────────────────────────────────────┘

   ┌──────────────┐      MQTT        ┌──────────────┐
   │   ESP32      │   (JSON a cada   │   Node-RED   │
   │  + Sensores  │────► 5 segundos) │   (Gateway)  │
   │   (Wokwi)    │                  │              │
   └──────────────┘                  └──────┬───────┘
         │                                  │
         │                                  │ HTTP
         ▼                                  ▼
   ┌──────────────┐                  ┌──────────────┐
   │   Display    │                  │   Dashboard  │
   │   LCD 16x2   │                  │     Web      │
   │   + LEDs     │                  │              │
   └──────────────┘                  └──────────────┘
```

## 🗣️ Fluxo de comunicação

1. ESP32 coleta dados dos sensores (temperatura, luz, ruído, obstrução)
2. Calcula o **Score de Acessibilidade** baseado em limiares pré-definidas
3. Publica dados via **MQTT** no broker público `broker.hivemq.com`
4. Node-RED recebe, processa e exibe os dados no dashboard
5. Dashboard atualiza em **tempo real** (a cada 5 segundos)

## ⚙️ Componentes de hardware

Segue abaixo a lista de componentes (Simulação Wokwi)

| Componente             | Modelo             | Função                              | Pino ESP32       |
| ---------------------- | ------------------ | ----------------------------------- | ---------------- |
| Microcontrolador       | ESP32 DevKit       | Processamento e conectividade WiFi  | -                |
| Sensor de Temperatura  | DHT22              | Mede temperatura (°C) e umidade (%) | GPIO 15          |
| Sensor de Luminosidade | LDR (Fotoresistor) | Mede intensidade de luz (0-4095)    | GPIO 34 (ADC)    |
| Simulador de Ruído     | Potenciômetro 10kΩ | Simula nível de ruído (dB)          | GPIO 35 (ADC)    |
| Detector de Obstrução  | Botão Push         | Simula obstrução no caminho         | GPIO 32          |
| Display LCD            | LCD 16x2 I2C       | Exibe score e status localmente     | SDA: 21, SCL: 22 |
| LED Verde              | LED 5mm            | Indica status OK (score ≥ 80%)      | GPIO 25          |
| LED Amarelo            | LED 5mm            | Indica status ALERTA (50-79%)       | GPIO 26          |
| LED Vermelho           | LED 5mm            | Indica status CRÍTICO (< 50%)       | GPIO 27          |

![Acessly_Circuito](https://drive.google.com/uc?export=view&id=1nh306tniZEnz4xbTe8ro9RqLonX7dhfp)

### 🪄 Requisitos técnicos

| Requisito      | Status     | Implementação                           |
| -------------- | ---------- | --------------------------------------- |
| Sistema IoT    |  ✅         | ESP32 com 5 sensores/atuadores          |
| Hardware       |  ✅          | ESP32 simulado no Wokwi                 |
| Dashboard      |  ✅          | Node-RED Dashboard responsivo           |
| Gateway        |  ✅          | Node-RED processa e exibe dados         |
| Protocolo MQTT |  ✅          | Broker HiveMQ, JSON, tópicos exclusivos |
| Tempo Real     |  ✅          | Atualização a cada 5 segundos           |

## 👨‍💻 Tecnologias utilizadas

- Linguagem C++
- Bibliotecas:
  
    - `WiFi.h` - conectividade WiFi
    - `PubSubClient.h` - cliente MQTT
    - `ArduinoJson.h` - manipulação JSON
    - `DHTesp.h` - sensor DHT22
    - `LiquidCrystal_I2C.h` - display LCD
    - `Wire.h` - comunicação I2C

- Gateway e Dashboard
  
    - Plataforma: Node-RED
    - Protocolo: MQTT (broquer.hivemq.com:1883)
    - Visualização: node-red-dashboard
    - Formato de dados: JSON
 
## 🪢 Fluxo Node-RED

![Acessly_Fluxo](https://drive.google.com/uc?export=view&id=1-uMNRWz_qEYX5tHRyhVUAAjyPjnRI72J)

### Descrição dos nodes

| Node                       | Tipo        | Função                                             |
| -------------------------- | ----------- | -------------------------------------------------- |
| MQTT Acessly               | mqtt-in     | Recebe dados do broker HiveMQ                      |
| json                       | json        | Converte string JSON em objeto                     |
| Separar Dados              | function    | Divide dados em 3 saídas (score, sensores, status) |
| Score de Acessibilidade    | gauge       | Exibe gauge visual com cores                       |
| Status                     | template    | Exibe badge de status                              |
| Preparar Séries do Gráfico | function    | Formata dados para o gráfico                       |
| Gráfico Sensores Ambientais          | chart       | Exibe linhas temporais                   |
| Cards                      | template    | Exibe 3 cards com valores dos sensores             |

## 📊 Fluxos de dados

### Tópicos MQTT

| Tópico                      | Finalidade                         | Frequência        |
| --------------------------- | ---------------------------------- | ----------------- |
| /acessly/empresa/001/data   | Dados dos sensores e score         | A cada 5 segundos |
| /acessly/empresa/001/status | Status da conexão (online/offline) | Na conexão        |

## 📈 Lógica de cálculo do Score

O **Score de Acessibilidade** inicia em 100% e é penalizado conforme as condições:

| Condição               | Penalização | Critério              |
| ---------------------- | ----------- | --------------------- |
| Luminosidade Baixa     | -25 pontos  | Luz < 200 lux         |
| Ruído Excessivo        | -20 pontos  | Ruído > 600 dB        |
| Temperatura Inadequada | -15 pontos  | Temp < 20°C ou > 24°C |
| Obstrução Detectada    | -40 pontos  | Caminho bloqueado     |

### Classificação do Score

| Score                  | Status      |  LED                  | Cor Dashboard         |
| ---------------------- | ----------- | --------------------- | --------------------- |
| 80% - 100%             | OK          | 🟢 Verde             | Verde                  |
| 50% - 79%              | WARNING     | 🟡 Amarelo           | Amarelo                |
|  0% - 49%              | CRITICAL    | 🔴 Vermelho          | Vermelho               |

## 📊 Dashboard

Acesso

```
http://localhost:1880/ui
```

## 🛞 Configuração e instalação

### Pré-requisitos
  - Wokwi
  - Node.js (v14 ou superior)
  - Node-RED instalado

### Passo 1: acessar o circuito no Wokwi

O projeto está disponível online pode ser acessado diretamente sem necessidade de criar conta no Wokwi.

1. Acesse o link do projeto **[🔗 Acessly Monitor no Wokwi](https://wokwi.com/projects/447344618432877569)**

2. Clique no botão **▶️ Play** para iniciar a simulação

3. Aguarde a conexão WiFi e MQTT (acompanhe no Serial Monitor)

4. O circuito começará a publicar dados automaticamente

### Passo 2: instalar Node-RED

No Windows/Linux/Mac:

```bash
# Instalar Node-RED globalmente
npm install -g node-red
```

```bash
# Instalar dashboard
npm install -g node-red-dashboard
```

### Passo 3: configurar Node-RED

3.1. Iniciar o Node-RED

```bash
node-red
```

Aguarde até ver a mensagem:

```
[info] Server now running at http://127.0.0.1:1880/
```

3.2. Acessar a interface

Abra o navegador em `http:localhost:1880`

3.3. Importar o Fluxo

  - No canto superior direito, clique no menu ≡ (três linhas)

  - Selecione Import → Clipboard

  - Cole o conteúdo do arquivo flows.json

  - Clique em Import

3.4. Verificar configuração MQTT

  - Dê dois cliques no node "MQTT Acessly" (roxo)
  - Verifique se o servidor está configurado como:
        - Server: `broker.hivemq.com`
        - Port: `1883`
        - Topic: /acessly/empresa/001/data
  - Clique em **Done**

3.5. Deploy

Clique no botão vermelho **Deploy** no canto superior direito

Aguarde a confirmação: "Successfully deployed"

### Passo 4: acessar o Dashboard

Abra o navegador em: http://localhost:1880/ui

Você verá o dashboard com:

  - ✅ Score de Acessibilidade (gauge)

  - ✅ Status do Ambiente (badge colorido)

  - ✅ Cards de Sensores (temperatura, luz, ruído)

  - ✅ Gráfico temporal com histórico
