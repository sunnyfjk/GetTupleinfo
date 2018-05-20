# @Author: fjk
# @Date:   2018-05-18T14:46:15+08:00
# @Last modified by:   fjk
# @Last modified time: 2018-05-20T15:37:53+08:00
.PHONY:$(TAGET)
TAGET 		=	GetTuple
SRC			=	$(shell find ./ -name "*.c")
OBJ_O 		=	${patsubst %.c,%.o,$(SRC)}
OBJ_D 		=	${patsubst %.c,%.d,$(SRC)}
LIBS			= -lpthread
C_FLAGS += -D__DEBUG__
$(TAGET):$(OBJ_O)
	gcc -o $@ $^ $(LIBS)
.c.o:
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" $(C_FLAGS)
clean:
	rm -rf $(OBJ_O) $(OBJ_D) $(TAGET)
