using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using RideWithMe.BL.Models;
using RideWithMe.Common.Enums;

namespace RideWithMe.App.Wrappers;

public class CarWrapper : ModelWrapper<CarDetailModel>
{
    public CarWrapper(CarDetailModel model) : base(model)
    {

    }

    public Guid OwnerId
    {
        get => GetValue<Guid>();
        set => SetValue(value);
    }

    public DateTime? RegistrationDate
    {
        get => GetValue<DateTime>();
        set => SetValue(value);
    }

    public CarTypeId CarTypeId
    {
        get => GetValue<CarTypeId>();
        set => SetValue(value);
    }

    public CarBrandId BrandId
    {
        get => GetValue<CarBrandId>();
        set => SetValue(value);
    }

    public UserModel? Owner
    {
        get => GetValue<UserModel>();
        set => SetValue(value);
    }

    public string? ImageUrl
    {
        get => GetValue<string>();
        set => SetValue(value);
    }

    public int Seats
    {
        get => GetValue<int>();
        set => SetValue(value);
    }

    public override IEnumerable<ValidationResult> Validate(ValidationContext validationContext)
    {
        if (Seats <= 0)
        {
            yield return new ValidationResult($"{nameof(Seats)} is required", new[] { nameof(Seats)});
        }

        if (CarTypeId == CarTypeId.None)
        {
            yield return new ValidationResult($"{nameof(CarTypeId)} is required", new[] { nameof(CarTypeId) });
        }

        if (BrandId == CarBrandId.None)
        {
            yield return new ValidationResult($"{nameof(BrandId)} is required", new[] { nameof(BrandId) });
        }
    }

    public static implicit operator CarWrapper(CarDetailModel carDetailModel) => new(carDetailModel);

    public static implicit operator CarDetailModel(CarWrapper wrapper) => wrapper.Model;
}