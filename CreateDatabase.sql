CREATE DATABASE [TestDb]
 
GO

USE [TestDb]
GO
/****** Object:  Table [dbo].[Order]    Script Date: 27.05.2013 00:12:34 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Order](
	[Id] [int] IDENTITY(1,1) NOT NULL,
	[OrderName] [nvarchar](50) NULL,
	[OrderDate] [datetime] NULL,
	[Total] [decimal](18, 2) NULL,
 CONSTRAINT [PK_Order] PRIMARY KEY CLUSTERED 
(
	[Id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

INSERT INTO dbo.[Order] (OrderName, OrderDate, Total)
VALUES
('Order1', '20130125 13:27:55.000', 237.34),
('Order2', '20121014 23:48:01.000', 110.50)

