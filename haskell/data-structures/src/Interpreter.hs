{-# LANGUAGE GADTs #-}

module Interpreter
    ( Expr
    , eval
    ) where

data Expr where
    Literal :: Integer -> Expr
    Add :: Expr -> Expr -> Expr
    Div :: Expr -> Expr -> Expr

eval :: Expr -> Maybe Integer
eval (Literal i) = Just i
eval (Add i j) = case (eval i, eval j) of
    (Just ei, Just ej) -> Just (ei + ej)
    (_, _) -> Nothing
eval (Div i j) = case (eval i, eval j) of
    (_, Nothing) -> Nothing
    (_, Just 0) -> Nothing
    (Just ei, Just ej) -> Just (div ei ej)
    (_, _) -> Nothing

divByZeroIsNothing :: Maybe Integer
divByZeroIsNothing =
    eval (Div (Add (Literal 3) (Literal (-2))) (Div (Literal 1) (Literal 2)))
-- >>> divByZeroIsNothing
-- Nothing

addingToDivByZeroIsNothing :: Maybe Integer
addingToDivByZeroIsNothing =
    eval (Add (Literal 3) (Div (Literal 1) (Literal 0)))
-- >>> addingToDivByZeroIsNothing
-- Nothing

dividingByDivByZeroIsNothing :: Maybe Integer
dividingByDivByZeroIsNothing =
    eval (Div (Literal 3) (Div (Literal 1) (Literal 0)))
-- >>> dividingByDivByZeroIsNothing
-- Nothing

dividingZeroBySomethingIsZero :: Maybe Integer
dividingZeroBySomethingIsZero =
    eval (Div (Literal 0) (Div (Literal 5) (Literal (-2))))
-- >>> dividingZeroBySomethingIsZero
-- Just 0

someAddsAndDividesGiveExpectedResult :: Maybe Integer
someAddsAndDividesGiveExpectedResult =
    eval
        (Add
            (Div (Literal (-4)) (Literal 2))
            (Div (Add (Literal 11) (Literal 6)) (Literal 3)))
-- >>> someAddsAndDividesGiveExpectedResult
-- Just 3
