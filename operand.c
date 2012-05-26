/*  C Code Generator
 *
 *  Copyright (c) 2012, Antoine Balestrat, merkil@savhon.org
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ccg.h"

Operand *pickRandomOperand(VariableList *scope)
{
    Variable *var;
    Operand *operand = xmalloc(sizeof(*operand));

    operand->type = rand() % 3 == 0 ? _constant : _variable;

    if(operand->type == _variable)
        operand->op.variable = pickRandomVariable(scope);
    else
        operand->op.constant = makeRandomIntegerConstant(RANDOM_BITNESS);

    return operand;
}