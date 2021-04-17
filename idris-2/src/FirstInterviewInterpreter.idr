module FirstInterviewInterpreter

import Decidable.Equality

%default total

data
Expr : Type where
    Literal : Integer -> Expr
    Add : Expr -> Expr -> Expr
    Div : Expr -> Expr -> Expr

eval : Expr -> Maybe Integer
eval (Literal i) = Just i
eval (Add i j) = [| eval i + eval j |]
eval (Div i j) = case !(eval j) of
    0 => Nothing
    ej => Just (div !(eval i) ej)

divByZeroIsNothing :
    eval (Div (Add (Literal 3) (Literal (-2))) (Div (Literal 1) (Literal 2))) =
        Nothing
divByZeroIsNothing = Refl

addingToDivByZeroIsNothing :
    eval (Add (Literal 3) (Div (Literal 1) (Literal 0))) = Nothing
addingToDivByZeroIsNothing = Refl

dividingByDivByZeroIsNothing :
    eval (Div (Literal 3) (Div (Literal 1) (Literal 0))) = Nothing
dividingByDivByZeroIsNothing = Refl

dividingZeroBySomethingIsZero :
    eval (Div (Literal 0) (Div (Literal 5) (Literal (-2)))) = Just 0
dividingZeroBySomethingIsZero = Refl

someAddsAndDividesGiveExpectedResult :
    eval
        (Add
            (Div (Literal (-4)) (Literal 2))
            (Div (Add (Literal 11) (Literal 6)) (Literal 3))) =
        Just 3
someAddsAndDividesGiveExpectedResult = Refl
