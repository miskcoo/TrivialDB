#include <cassert>
#include "expression.h"

inline expression eval_terminal(const expr_node_t *expr)
{
	expression ret;
	ret.type = expr->term_type;
	switch(expr->term_type)
	{
		case TERM_INT:
			ret.val_i = expr->val_i;
			break;
		case TERM_FLOAT:
			ret.val_f = expr->val_f;
			break;
		case TERM_STRING:
			ret.val_s = expr->val_s;
			break;
		case TERM_NULL:
			break;
		case TERM_COLUMN_REF:
			// TODO: load column info
			break;
		default:
			assert(0);
			break;
	}

	return ret;
}

inline expression eval_float_operands(operator_type_t op, float a, float b)
{
	expression ret;
	switch(op)
	{
		case OPERATOR_ADD:
			ret.val_f = a + b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_MINUS:
			ret.val_f = a - b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_DIV:
			ret.val_f = a / b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_MUL:
			ret.val_f = a * b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_NEGATE:
			ret.val_f = -a;
			ret.type  = TERM_INT;
			break;
		default:
			assert(0);
			// TODO: report error
			break;
	}

	return ret;
}

inline expression eval_int_operands(operator_type_t op, int a, int b)
{
	expression ret;
	switch(op)
	{
		case OPERATOR_ADD:
			ret.val_i = a + b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_MINUS:
			ret.val_i = a - b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_DIV:
			ret.val_i = a / b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_MUL:
			ret.val_i = a * b;
			ret.type  = TERM_INT;
			break;
		case OPERATOR_NEGATE:
			ret.val_i = -a;
			ret.type  = TERM_INT;
			break;
		default:
			assert(0);
			// TODO: report error
			break;
	}

	return ret;
}

expression expression::eval(const expr_node_t *expr)
{
	assert(expr != nullptr);
	if(expr->op == OPERATOR_NONE)
	{
		// terminator
		return eval_terminal(expr);
	}

	assert(expr->term_type == TERM_NONE);

	// non-terminator
	bool is_unary = (expr->op & OPERATOR_UNARY);
	expression left  = eval(expr->left);
	expression right = is_unary ? expression() : eval(expr->right);

	if(!is_unary && right.type == TERM_NULL)
	{
		expression ret;
		ret.type = TERM_NULL;
		return ret;
	}

	if(!is_unary && left.type != right.type && left.type != TERM_NULL)
		throw "[Error] operand type mismatch.";
	
	switch(left.type)
	{
		case TERM_INT:
			return eval_int_operands(expr->op, left.val_i, right.val_i);
		case TERM_FLOAT:
			return eval_float_operands(expr->op, left.val_f, right.val_f);
		default:
			// TODO: implement STRING, BOOL, NULL
			assert(0);
			return expression();
	}
}
