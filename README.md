# SO
## Visão Geral
O presente trabalho consiste da implementação de um gerente de escalonamento postergado de processos. 
Vamos ter no mínimo dois processos: solicita execução e escalonador de processos.
## Descrição dos Processos:

### **1- Processo de solicitação de execução:** Será executado via shell.

**Sintaxe**: solicita_execucao(tempo, copias, prioridade, arquivo_exec)
    
> __tempo__: Parâmetro a ser fornecido no formato hh:mm. Especifica o delay de execução em relação à hora corrente.

>__copias__: Parâmetro a ser fornecido como um inteiro. Especifica o número de cópias que serão executadas.

>__prioridade__: Prioridade inicial do Processo. (1, 2 ou 3).

>__arquivo_exec__: nome do arquivo executável a ser executado de maneira postergada.

**Comportamento:**
>Se não houver parâmetro inválido, é atribuído um número do job único à tupla \<arquivo_exec>, 
\<tempo>, \<copias>, que é disponibilizada para o processo escalonador.
Ao final da execução, o processo solicita_execucao imprime o número do job e a tupla.

    Exemplo:
        executa_postergado 0:05 3 hello_world
            Admitindo que são 12:00, executa 3 cópias de hello_world às 12:05.

### **2 - Processo escalonador de processos:** Roda em background.
**Sintaxe:** escalonador &

**Comportamento:**
>O processo escalonador é o responsável pela execução postergada. Verifica os jobs submetidos à procura de processos a executar. Se o delay tiver sido cumprido, o escalonador recupera o nome do arquivo executável, cria cópias processos e os coloca para execução na fila de sua prioridade. O escalonador executa o processo que está no início da fila de maior prioridade pelo quantum de 5 segundos. Ao perder a CPU, a prioridade do processo é recalculada e o processo é inserido no final da fila de sua nova prioridade. A prioridade é recalculada da seguinte maneira: se o processo esteve duas vezes consecutivas na fila de prioridade i, ele desce para a fila de prioridade i-1 até chegar à fila de menor prioridade. Ao chegar à fila de menor prioridade (2 vezes), inverte-se o cálculo e o processo sobe de prioridade até que chegue à fila de menor prioridade.

    Exemplo:  um processo colocado na fila de prioridade 1 (mais alta) que tenha duração de 40s 
              executará da seguinte maneira:

              fila1(5s)
              fila1(5s)
              fila2(5s)
              fila2(5s)
              fila3(5s)
              fila3(5s)
              fila2(5s)
              fila2(5s)


#### Atenção:
    Os processos que estão prontos para execução em um dado momento são regidos pela política Round-Robin, 
    com quantum de 5 segundos. Em um dado momento, somente um dos processos regidos pelo escalonador estará 
    no estado RUNNING.

#### Exemplo:
Às 16:00 temos a seguinte configuração na estrutura de dados:

| job| arquivo_exec        | hh:mm           | copias  | prioridade |
| -- | ------------------- |:---------------:| -------:| ----------:|
| 5  | hello_world         | 12:05           |  3      |    2       |
| 7  | teste               | 12:30           |  1      |    1       |

