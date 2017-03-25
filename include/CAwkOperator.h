#ifndef CAWK_OPERATOR_H
#define CAWK_OPERATOR_H

class CAwkOperator : public CAwkExpressionTerm {
 public:
  enum class OpType {
    UNARY       = (1<<0),
    BINARY      = (1<<1),
    TERNARY     = (1<<2),
    START_GROUP = (1<<3),
    END_GROUP   = (1<<4),
    ASSIGN      = (1<<5),

    UNARY_ASSIGN  = (UNARY  | ASSIGN),
    BINARY_ASSIGN = (BINARY | ASSIGN)
  };

  enum class Direction {
    L_TO_R,
    R_TO_L
  };

 protected:
  friend class CRefPtr<CAwkOperator>;

  CAwkOperator() { }

  virtual ~CAwkOperator() { }

  CAwkOperator *dup() const { return NULL; }

 public:
  bool hasValue() const { return false; }

  CAwkValuePtr getValue() const { return CAwkValuePtr(); }

  virtual void print(std::ostream &os) const = 0;

  virtual OpType getType() const = 0;

  bool isUnary  () { return (int(getType()) & int(OpType::UNARY  )); }
  bool isBinary () { return (int(getType()) & int(OpType::BINARY )); }
  bool isTernary() { return (int(getType()) & int(OpType::TERNARY)); }

  virtual Direction getDirection() const = 0;

  virtual uint getPrecedence() const = 0;

  virtual CAwkExpressionTermPtr execute() = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkOperator &th) {
    th.print(os); return os;
  }
};

class CAwkUnaryOperator : public CAwkOperator {
 protected:
  CAwkUnaryOperator() { }

  virtual ~CAwkUnaryOperator() { }
};

class CAwkBinaryOperator : public CAwkOperator {
 protected:
  CAwkBinaryOperator() { }

  virtual ~CAwkBinaryOperator() { }
};

class CAwkAssignOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkAssignOperator);
  }

 private:
  CAwkAssignOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "="; }
};

class CAwkPlusEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPlusEqualsOperator);
  }

 private:
  CAwkPlusEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "+="; }
};

class CAwkMinusEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkMinusEqualsOperator);
  }

 private:
  CAwkMinusEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "-="; }
};

class CAwkTimesEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkTimesEqualsOperator);
  }

 private:
  CAwkTimesEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "*="; }
};

class CAwkDivideEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkDivideEqualsOperator);
  }

 private:
  CAwkDivideEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "/="; }
};

class CAwkModulusEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkModulusEqualsOperator);
  }

 private:
  CAwkModulusEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "%="; }
};

class CAwkPowerEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPowerEqualsOperator);
  }

 private:
  CAwkPowerEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 1; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "^="; }
};

class CAwkQuestionOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkQuestionOperator);
  }

 private:
  CAwkQuestionOperator() { }

 public:
  OpType getType() const { return OpType::TERNARY; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 2; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "?"; }
};

class CAwkColonOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkColonOperator);
  }

 private:
  CAwkColonOperator() { }

 public:
  OpType getType() const { return OpType::TERNARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 2; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << ":"; }
};

class CAwkLogicalOrOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLogicalOrOperator);
  }

 private:
  CAwkLogicalOrOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 3; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "||"; }
};

class CAwkLogicalAndOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLogicalAndOperator);
  }

 private:
  CAwkLogicalAndOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 4; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "&&"; }
};

class CAwkInOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkInOperator);
  }

 private:
  CAwkInOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 5; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "in"; }
};

class CAwkRegExpOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkRegExpOperator);
  }

 private:
  CAwkRegExpOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 6; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "~"; }
};

class CAwkNotRegExpOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkNotRegExpOperator);
  }

 private:
  CAwkNotRegExpOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 6; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "!~"; }
};

class CAwkLessOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLessOperator);
  }

 private:
  CAwkLessOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 7; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "<"; }
};

class CAwkLessEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLessEqualsOperator);
  }

 private:
  CAwkLessEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 7; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "<="; }
};

class CAwkEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkEqualsOperator);
  }

 private:
  CAwkEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 7; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "=="; }
};

class CAwkNotEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkNotEqualsOperator);
  }

 private:
  CAwkNotEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 7; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "!="; }
};

class CAwkGreaterEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkGreaterEqualsOperator);
  }

 private:
  CAwkGreaterEqualsOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 7; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << ">="; }
};

class CAwkGreaterOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkGreaterOperator);
  }

 private:
  CAwkGreaterOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 7; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << ">"; }
};

class CAwkConcatOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkConcatOperator);
  }

 private:
  CAwkConcatOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 8; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "@"; }
};

class CAwkPlusOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPlusOperator);
  }

 private:
  CAwkPlusOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 9; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "+"; }
};

class CAwkMinusOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkMinusOperator);
  }

 private:
  CAwkMinusOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 9; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "-"; }
};

class CAwkTimesOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkTimesOperator);
  }

 private:
  CAwkTimesOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 10; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "*"; }
};

class CAwkDivideOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkDivideOperator);
  }

 private:
  CAwkDivideOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 10; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "/"; }
};

class CAwkModulusOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkModulusOperator);
  }

 private:
  CAwkModulusOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 10; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "%"; }
};

class CAwkUnaryPlusOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkUnaryPlusOperator);
  }

 private:
  CAwkUnaryPlusOperator() { }

 public:
  OpType getType() const { return OpType::UNARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 11; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "+"; }
};

class CAwkUnaryMinusOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkUnaryMinusOperator);
  }

 private:
  CAwkUnaryMinusOperator() { }

 public:
  OpType getType() const { return OpType::UNARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 11; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "-"; }
};

class CAwkLogicalNotOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLogicalNotOperator);
  }

 private:
  CAwkLogicalNotOperator() { }

 public:
  OpType getType() const { return OpType::UNARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 12; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "!"; }
};

class CAwkPowerOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPowerOperator);
  }

 private:
  CAwkPowerOperator() { }

 public:
  OpType getType() const { return OpType::BINARY; }

  Direction getDirection() const { return Direction::R_TO_L; }

  uint getPrecedence() const { return 13; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "^"; }
};

class CAwkPreIncrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPreIncrementOperator);
  }

 private:
  CAwkPreIncrementOperator() { }

 public:
  OpType getType() const { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 14; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "++"; }
};

class CAwkPostIncrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPostIncrementOperator);
  }

 private:
  CAwkPostIncrementOperator() { }

 public:
  OpType getType() const { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 14; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "++"; }
};

class CAwkPreDecrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPreDecrementOperator);
  }

 private:
  CAwkPreDecrementOperator() { }

 public:
  OpType getType() const { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 14; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "--"; }
};

class CAwkPostDecrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPostDecrementOperator);
  }

 private:
  CAwkPostDecrementOperator() { }

 public:
  OpType getType() const { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 14; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "--"; }
};

class CAwkFieldOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkFieldOperator);
  }

 private:
  CAwkFieldOperator() { }

 public:
  OpType getType() const { return OpType::UNARY; }

  Direction getDirection() const { return Direction::L_TO_R; }

  uint getPrecedence() const { return 15; }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const { os << "$"; }
};

#endif
