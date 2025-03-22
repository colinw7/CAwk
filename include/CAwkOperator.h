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
  CAwkOperator() { }

  CAwkOperator *dup() const { return NULL; }

 public:
  virtual ~CAwkOperator() { }

  bool hasValue() const override { return false; }

  CAwkValuePtr getValue() const override { return CAwkValuePtr(); }

  void print(std::ostream &os) const override = 0;

  virtual OpType getType() const = 0;

  bool isUnary  () { return (int(getType()) & int(OpType::UNARY  )); }
  bool isBinary () { return (int(getType()) & int(OpType::BINARY )); }
  bool isTernary() { return (int(getType()) & int(OpType::TERNARY)); }

  virtual Direction getDirection() const = 0;

  virtual uint getPrecedence() const = 0;

  CAwkExpressionTermPtr execute() override = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkOperator &th) {
    th.print(os); return os;
  }
};

//---

class CAwkUnaryOperator : public CAwkOperator {
 protected:
  CAwkUnaryOperator() { }

 public:
  virtual ~CAwkUnaryOperator() { }
};

//---

class CAwkBinaryOperator : public CAwkOperator {
 protected:
  CAwkBinaryOperator() { }

 public:
  virtual ~CAwkBinaryOperator() { }
};

//---

class CAwkAssignOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkAssignOperator);
  }

 private:
  CAwkAssignOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "="; }
};

//---

class CAwkPlusEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPlusEqualsOperator);
  }

 private:
  CAwkPlusEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "+="; }
};

//---

class CAwkMinusEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkMinusEqualsOperator);
  }

 private:
  CAwkMinusEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "-="; }
};

//---

class CAwkTimesEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkTimesEqualsOperator);
  }

 private:
  CAwkTimesEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "*="; }
};

//---

class CAwkDivideEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkDivideEqualsOperator);
  }

 private:
  CAwkDivideEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "/="; }
};

//---

class CAwkModulusEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkModulusEqualsOperator);
  }

 private:
  CAwkModulusEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "%="; }
};

//---

class CAwkPowerEqualsOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPowerEqualsOperator);
  }

 private:
  CAwkPowerEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY_ASSIGN; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 1; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "^="; }
};

//---

class CAwkQuestionOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkQuestionOperator);
  }

 private:
  CAwkQuestionOperator() { }

 public:
  OpType getType() const override { return OpType::TERNARY; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 2; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "?"; }
};

//---

class CAwkColonOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkColonOperator);
  }

 private:
  CAwkColonOperator() { }

 public:
  OpType getType() const override { return OpType::TERNARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 2; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << ":"; }
};

//---

class CAwkLogicalOrOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLogicalOrOperator);
  }

 private:
  CAwkLogicalOrOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 3; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "||"; }
};

//---

class CAwkLogicalAndOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLogicalAndOperator);
  }

 private:
  CAwkLogicalAndOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 4; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "&&"; }
};

//---

class CAwkInOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkInOperator);
  }

 private:
  CAwkInOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 5; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "in"; }
};

//---

class CAwkRegExpOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkRegExpOperator);
  }

 private:
  CAwkRegExpOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 6; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "~"; }
};

//---

class CAwkNotRegExpOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkNotRegExpOperator);
  }

 private:
  CAwkNotRegExpOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 6; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "!~"; }
};

//---

class CAwkLessOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLessOperator);
  }

 private:
  CAwkLessOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 7; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "<"; }
};

//---

class CAwkLessEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLessEqualsOperator);
  }

 private:
  CAwkLessEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 7; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "<="; }
};

//---

class CAwkEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkEqualsOperator);
  }

 private:
  CAwkEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 7; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "=="; }
};

//---

class CAwkNotEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkNotEqualsOperator);
  }

 private:
  CAwkNotEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 7; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "!="; }
};

//---

class CAwkGreaterEqualsOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkGreaterEqualsOperator);
  }

 private:
  CAwkGreaterEqualsOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 7; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << ">="; }
};

//---

class CAwkGreaterOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkGreaterOperator);
  }

 private:
  CAwkGreaterOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 7; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << ">"; }
};

//---

class CAwkConcatOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkConcatOperator);
  }

 private:
  CAwkConcatOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 8; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "@"; }
};

//---

class CAwkPlusOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPlusOperator);
  }

 private:
  CAwkPlusOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 9; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "+"; }
};

//---

class CAwkMinusOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkMinusOperator);
  }

 private:
  CAwkMinusOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 9; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "-"; }
};

//---

class CAwkTimesOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkTimesOperator);
  }

 private:
  CAwkTimesOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 10; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "*"; }
};

//---

class CAwkDivideOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkDivideOperator);
  }

 private:
  CAwkDivideOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 10; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "/"; }
};

//---

class CAwkModulusOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkModulusOperator);
  }

 private:
  CAwkModulusOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 10; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "%"; }
};

//---

class CAwkUnaryPlusOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkUnaryPlusOperator);
  }

 private:
  CAwkUnaryPlusOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 11; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "+"; }
};

//---

class CAwkUnaryMinusOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkUnaryMinusOperator);
  }

 private:
  CAwkUnaryMinusOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 11; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "-"; }
};

//---

class CAwkLogicalNotOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkLogicalNotOperator);
  }

 private:
  CAwkLogicalNotOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 12; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "!"; }
};

//---

class CAwkPowerOperator : public CAwkBinaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPowerOperator);
  }

 private:
  CAwkPowerOperator() { }

 public:
  OpType getType() const override { return OpType::BINARY; }

  Direction getDirection() const override { return Direction::R_TO_L; }

  uint getPrecedence() const override { return 13; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "^"; }
};

//---

class CAwkPreIncrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPreIncrementOperator);
  }

 private:
  CAwkPreIncrementOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 14; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "++"; }
};

//---

class CAwkPostIncrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPostIncrementOperator);
  }

 private:
  CAwkPostIncrementOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 14; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "++"; }
};

//---

class CAwkPreDecrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPreDecrementOperator);
  }

 private:
  CAwkPreDecrementOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 14; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "--"; }
};

//---

class CAwkPostDecrementOperator : public CAwkOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkPostDecrementOperator);
  }

 private:
  CAwkPostDecrementOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY_ASSIGN; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 14; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "--"; }
};

//---

class CAwkFieldOperator : public CAwkUnaryOperator {
 public:
  static CAwkOperatorPtr create() {
    return CAwkOperatorPtr(new CAwkFieldOperator);
  }

 private:
  CAwkFieldOperator() { }

 public:
  OpType getType() const override { return OpType::UNARY; }

  Direction getDirection() const override { return Direction::L_TO_R; }

  uint getPrecedence() const override { return 15; }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override { os << "$"; }
};

#endif
