# SO
## Visão Geral
    O presente trabalho consiste da implementação de um gerente de escalonamento postergado de processos. 
    Vamos ter no mínimo dois processos: solicita execução e escalonador de processos.
## Descrição dos Processos:

### **Processo de solicitação de execução:** Será executado via shell.
    a) Sintaxe: solicita_execucao(tempo, copias, prioridade, arquivo_exec)
        
        tempo: Parâmetro a ser fornecido no formato hh:mm. Especifica o delay de execução em relação à hora 
        corrente.
        copias: Parâmetro a ser fornecido como um inteiro. Especifica o número de cópias que serão executadas.
        prioridade: Prioridade inicial do Processo. (1, 2 ou 3).
        arquivo_exec: nome do arquivo executável a ser executado de maneira postergada.
    
    b)Comportamento:
        Se não houver parâmetro inválido, é atribuído um número do job único à tupla <arquivo_exec>, <tempo>, 
        <copias>, que é disponibilizada para o processo escalonador.
        Ao final da execução, o processo solicita_execucao imprime o número do job e a tupla.